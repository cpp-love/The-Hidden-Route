/**
 * @file test_level_serialization_system.cpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 添加了 `thr::ecs::level_serialization_system` 的测试用例和使用示例。
 * @version 0.1.0-1
 * @date 2026-05-23
 * 
 * @copyright cpp-love
 * 
 */

#include "thr/ecs/components/maze_components.hpp"
#include "thr/ecs/systems/global/game_state_manager.hpp"
#include "thr/ecs/systems/level_render_system.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <iostream>
#include <memory>
#undef NDEBUG
#include "thr/ecs/systems/level_serialization_system.hpp"
#include <entt/entity/registry.hpp>
#include <format>
#include <fstream>
#include <nlohmann/json.hpp>
#include <print>
#include <thr/base/assert_msg.hpp>

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

class test_state : public thr::ecs::game_state_base {
  public:
    test_state() noexcept {
        thr::ecs::segment::connect_listener(m_registry);
        thr::ecs::node::connect_listener(m_registry);
        std::print("请输入新关卡的 json 文件：");
        (void)std::fflush(stdout);
        std::filesystem::path path;
        std::cin >> path;
        std::ifstream fin(path);
        if (!fin.is_open()) {
            std::println("Error: could not open the file maze.json");
            exit(-1);
        }
        nlohmann::json json;
        fin >> json;
        thr::ecs::level_serialization_system::deserialize_from_json(m_registry, json);
        nlohmann::json json2 = thr::ecs::level_serialization_system::serialize_to_json(m_registry);
        std::println("json: {}", nlohmann::to_string(json));
        std::println("converted json: {}", nlohmann::to_string(json2));
    }
    void on_pause() noexcept override {}
    void on_resume() noexcept override {}
    bool handle_event([[maybe_unused]] const sf::Event &event) noexcept override {
        if (const auto *key_pressed = event.getIf<sf::Event::KeyPressed>();
            key_pressed != nullptr && key_pressed->code == sf::Keyboard::Key::R) {
            std::print("请输入新关卡的 json 文件：");
            (void)std::fflush(stdout);
            std::filesystem::path path;
            std::cin >> path;
            std::ifstream fin(path);
            if (!fin.is_open()) {
                std::println("Error: could not open the file maze.json");
                return true;
            }
            nlohmann::json json;
            fin >> json;
            thr::ecs::segment::disconnect_listener(m_registry);
            thr::ecs::node::disconnect_listener(m_registry);
            m_registry.clear();
            m_registry.ctx().clear();
            thr::ecs::segment::connect_listener(m_registry);
            thr::ecs::node::connect_listener(m_registry);
            thr::ecs::level_serialization_system::deserialize_from_json(m_registry, json);
            nlohmann::json json2 = thr::ecs::level_serialization_system::serialize_to_json(m_registry);
            std::println("json: {}", nlohmann::to_string(json));
            std::println("converted json: {}", nlohmann::to_string(json2));
            return true;
        }
        return false;
    }
    /// @copydoc game_state_base::update
    void update([[maybe_unused]] thr::ecs::milliseconds_f delta_time) noexcept override {}
    /// @copydoc game_state_base::draw
    void draw() noexcept override { thr::ecs::level_render_system::draw(m_registry, *m_window); }

  private:
    entt::registry m_registry; ///< 注册表
};

constexpr sf::Vector2u video_mode{1000, 700};

int                    main() {

#ifdef _WIN32
    // 让Windows支持UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif // _WIN32

    sf::RenderWindow             window(sf::VideoMode(video_mode), "test maze serialization system");
    thr::ecs::game_state_manager manager(window);
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
        manager.draw();
        window.display();
    }

    return 0;
}