/**
 * @file test_maze_render_system.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief `thr::ecs::maze_render_system` 的测试用例和使用示例。
 * @version 0.1.0-1
 * @date 2026-05-03
 * 
 * @copyright cpp-love
 * 
 */

#include "thr/ecs/components/global/game_state_components.hpp"
#include "thr/ecs/components/maze_line_components.hpp"
#include "thr/ecs/systems/global/game_state_manager.hpp"
#include "thr/ecs/systems/maze_render_system.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <entt/entity/registry.hpp>
#include <memory>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
class test_state : public thr::ecs::game_state_base {
  public:
    test_state() noexcept {
        m_registry.ctx().emplace<thr::ecs::line_strips>(
            std::vector<std::vector<sf::Vector2f>>{{
                                                       {1, 2},
                                                       {300, 200},
                                                       {100, 200},
                                                   },
                                                   {{100, 2}, {500, 200}, {400, 100}},
                                                   {{1, 200}, {300, 400}, {600, 550}}},
            sf::Color::Red);
        entt::entity entity = m_registry.create();
        m_registry.emplace<thr::ecs::segment>(entity, thr::ecs::segment{.start_center = {600, 600},
                                                                        .length = 100.f,
                                                                        .walked_precent = 0.5f,
                                                                        .dir = thr::ecs::direction::up});
        entt::entity entity2 = m_registry.create();
        m_registry.emplace<thr::ecs::segment>(entity2,
                                              thr::ecs::segment{.start_center = {250, 400},
                                                                .length = 100.f,
                                                                .walked_precent = 0.5f,
                                                                .dir = thr::ecs::direction::right});
    }

    void on_pause() noexcept override {}
    void on_resume() noexcept override {}
    bool handle_event([[maybe_unused]] const sf::Event &event) noexcept override { return false; }
    /// @copydoc game_state_base::update
    void update([[maybe_unused]] thr::ecs::milliseconds_f delta_time) noexcept override {}
    /// @copydoc game_state_base::draw
    void draw(sf::RenderTarget &render) noexcept override {
        thr::ecs::maze_render_system::draw(m_registry, render);
    }

  private:
    entt::registry m_registry; ///< 注册表
};
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

constexpr sf::Vector2u video_mode{800, 600};

int                    main() {

#ifdef _WIN32
    // 让Windows支持UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif // _WIN32

    // spdlog config
    auto file_logger = std::make_shared<spdlog::sinks::basic_file_sink_st>("log/log.log");
    file_logger->set_level(spdlog::level::trace);
    auto stdout_logger = std::make_shared<spdlog::sinks::stdout_sink_st>();
    stdout_logger->set_level(spdlog::level::info);
    auto multi_sink_logger = std::make_shared<spdlog::logger>(
        "multi_sink_logger", spdlog::sinks_init_list{file_logger, stdout_logger});
    multi_sink_logger->set_level(spdlog::level::trace);
    spdlog::set_default_logger(multi_sink_logger);
    spdlog::flush_on(spdlog::level::info);

    sf::RenderWindow             window(sf::VideoMode(video_mode), "test game state manager");
    thr::ecs::game_state_manager manager;
    manager.push_state(std::make_unique<test_state>());

    auto prev = thr::ecs::clock::now();
    while (window.isOpen()) {
        // handle event
        while (const std::optional event = window.pollEvent()) {
            if (!manager.handle_event(*event)) {
                if (event->is<sf::Event::Closed>()) {
                    spdlog::info("quit the window");
                    window.close();
                }
            }
        }

        // update
        auto cur = thr::ecs::clock::now();
        manager.update(cur - prev);
        prev = cur;

        // render
        window.clear();
        manager.draw(window);
        window.display();
    }

    return 0;
}