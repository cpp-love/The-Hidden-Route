/**
 * @file game_states.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 实现了一些具体的游戏状态。
 * @version 0.1.0-1
 * @date 2026-05-02
 * 
 * @copyright cpp-love
 * 
 */

#include "game_states.hpp"
#include "thr/ecs/systems/global/game_state_manager.hpp"
#include "thr/ecs/systems/maze_render_system.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <entt/signal/dispatcher.hpp>
#include <spdlog/spdlog.h>

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
namespace mainhelper {

    // settings_menu
    settings_menu::settings_menu() noexcept { connect_dispatcher(); }

    void settings_menu::on_pause() noexcept {
        spdlog::trace("settings_menu paused");
        m_is_paused = true;
    }
    void settings_menu::on_resume() noexcept {
        spdlog::trace("settings_menu resumed");
        m_is_paused = false;
    }
    bool settings_menu::handle_event(const sf::Event &event) noexcept {
        spdlog::trace("settings_menu handled event");
        if (m_is_paused) {
            return false;
        }
        if (const auto *key_pressed = event.getIf<sf::Event::KeyPressed>()) {
            if (key_pressed->code == sf::Keyboard::Key::Escape) {
                m_outside_dispather->enqueue<thr::ecs::game_state_pop_event>();
                return true;
            }
        }
        return false;
    }
    void settings_menu::update(thr::ecs::milliseconds_f delta_time) noexcept {
        spdlog::trace("settings_menu updated, time: {}", delta_time);
    }
    void settings_menu::draw(sf::RenderTarget &render) noexcept {
        spdlog::trace("settings_menu drew");
        sf::RectangleShape rect{{200, 30}};
        render.draw(rect);
    }
    void settings_menu::connect_dispatcher() noexcept {}
    void settings_menu::disconnect_dispatcher() noexcept {}

    // main_menu
    main_menu::main_menu() noexcept { connect_dispatcher(); }

    void main_menu::on_pause() noexcept {
        spdlog::trace("main_menu paused");
        m_is_paused = true;
    }
    void main_menu::on_resume() noexcept {
        spdlog::trace("main_menu resumed");
        m_is_paused = false;
    }
    bool main_menu::handle_event(const sf::Event &event) noexcept {
        spdlog::trace("main_menu handled event");
        if (m_is_paused) {
            return false;
        }
        if (const auto *key_pressed = event.getIf<sf::Event::KeyPressed>()) {
            if (key_pressed->code == sf::Keyboard::Key::S) {
                m_outside_dispather->enqueue<thr::ecs::game_state_push_event>(
                    std::make_unique<settings_menu>());
                return true;
            }
            if (key_pressed->code == sf::Keyboard::Key::G) {
                m_outside_dispather->enqueue<thr::ecs::game_state_push_event>(
                    std::make_unique<game_screen>());
                return true;
            }
        }
        return false;
    }
    void main_menu::update(thr::ecs::milliseconds_f delta_time) noexcept {
        spdlog::trace("main_menu updated, time: {}", delta_time);
    }
    void main_menu::draw(sf::RenderTarget &render) noexcept {
        spdlog::trace("main_menu drew");
        sf::CircleShape circle{200};
        render.draw(circle);
    }
    void main_menu::connect_dispatcher() noexcept {}
    void main_menu::disconnect_dispatcher() noexcept {}

    // game_screen
    game_screen::game_screen() noexcept {
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
        connect_dispatcher();
    }

    void game_screen::on_pause() noexcept {
        spdlog::trace("game_screen paused");
        m_is_paused = true;
    }
    void game_screen::on_resume() noexcept {
        spdlog::trace("game_screen resumed");
        m_is_paused = false;
    }
    bool game_screen::handle_event(const sf::Event &event) noexcept {
        spdlog::trace("game_screen handled event");
        if (m_is_paused) {
            return false;
        }
        if (const auto *key_pressed = event.getIf<sf::Event::KeyPressed>()) {
            if (key_pressed->code == sf::Keyboard::Key::S) {
                m_outside_dispather->enqueue<thr::ecs::game_state_push_event>(
                    std::make_unique<settings_menu>());
                return true;
            }
            if (key_pressed->code == sf::Keyboard::Key::Escape) {
                m_outside_dispather->enqueue<thr::ecs::game_state_pop_event>();
                return true;
            }
            if (key_pressed->code == sf::Keyboard::Key::P) {
                m_outside_dispather->enqueue<thr::ecs::game_state_push_event>(
                    std::make_unique<pause_menu>());
                return true;
            }
        }
        return false;
    }
    void game_screen::update(thr::ecs::milliseconds_f delta_time) noexcept {
        spdlog::trace("game_screen updated, time: {}", delta_time);
    }
    void game_screen::draw(sf::RenderTarget &render) noexcept {
        spdlog::trace("game_screen drew");
        thr::ecs::maze_render_system::draw(m_registry, render);
    }
    void game_screen::connect_dispatcher() noexcept {}
    void game_screen::disconnect_dispatcher() noexcept {}

    // pause_menu
    pause_menu::pause_menu() noexcept { connect_dispatcher(); }

    void pause_menu::on_pause() noexcept {
        spdlog::trace("pause_menu paused");
        m_is_paused = true;
    }
    void pause_menu::on_resume() noexcept {
        spdlog::trace("pause_menu resumed");
        m_is_paused = false;
    }
    bool pause_menu::handle_event(const sf::Event &event) noexcept {
        spdlog::trace("pause_menu handled event");
        if (m_is_paused) {
            return false;
        }
        if (const auto *key_pressed = event.getIf<sf::Event::KeyPressed>()) {
            if (key_pressed->code == sf::Keyboard::Key::Escape) {
                m_outside_dispather->enqueue<thr::ecs::game_state_pop_event>();
                return true;
            }
        }
        return false;
    }
    void pause_menu::update(thr::ecs::milliseconds_f delta_time) noexcept {
        spdlog::trace("pause_menu updated, time: {}", delta_time);
    }
    void pause_menu::draw(sf::RenderTarget &render) noexcept {
        spdlog::trace("pause_menu drew");
        sf::CircleShape circle{100};
        render.draw(circle);
        sf::RectangleShape rect{{20, 200}};
        rect.setPosition({114, 514});
        render.draw(rect);
    }
    void pause_menu::connect_dispatcher() noexcept {}
    void pause_menu::disconnect_dispatcher() noexcept {}

} // namespace mainhelper
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)