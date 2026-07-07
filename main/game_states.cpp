/**
 * @file game_states.cpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 实现了一些具体的游戏状态。
 * @version 0.1.0-3
 * @date 2026-07-01
 * 
 * @copyright cpp-love
 * 
 * @details 暂时使用思源宋体作为字体。
 */

#include "game_states.hpp"
#include "thr/base/floating_point_compare.hpp"
#include "thr/ecs/components/global/game_base.hpp"
#include "thr/ecs/components/maze_components.hpp"
#include "thr/ecs/components/player_components.hpp"
#include "thr/ecs/systems/global/game_state_manager.hpp"
#include "thr/ecs/systems/level_render_system.hpp"
#include "thr/ecs/systems/level_serialization_system.hpp"
#include "thr/ecs/systems/player_movement_system.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/String.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/Font.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <algorithm>
#include <chrono>
#include <entt/entity/fwd.hpp>
#include <entt/signal/dispatcher.hpp>
#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace {

    const sf::Font &get_sfml_font() {
        static sf::Font font("font.otf");
        return font;
    };
    const tgui::Font &get_tgui_font() {
        static tgui::Font font("font.otf");
        return font;
    };

} // namespace

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
namespace mainhelper {

    // settings_menu
    settings_menu::settings_menu() noexcept { connect_dispatcher(); }
    settings_menu::~settings_menu() noexcept { disconnect_dispatcher(); }

    void settings_menu::on_pause() noexcept { m_is_paused = true; }
    void settings_menu::on_resume() noexcept { m_is_paused = false; }
    bool settings_menu::handle_event([[maybe_unused]] const sf::Event &event) noexcept { return false; }
    void settings_menu::update([[maybe_unused]] thr::ecs::milliseconds_f delta_time) noexcept {}
    void settings_menu::draw() noexcept {}
    void settings_menu::connect_dispatcher() noexcept {}
    void settings_menu::disconnect_dispatcher() noexcept {}

    // main_menu
    main_menu::main_menu() noexcept { connect_dispatcher(); }
    main_menu::~main_menu() noexcept {
        tgui::Button::Ptr start_button = m_global_gui->get<tgui::Button>("start_button");
        m_global_gui->remove(start_button);
        disconnect_dispatcher();
    }

    void main_menu::init() noexcept {
        m_global_gui->setFont(get_tgui_font());
        tgui::Button::Ptr start_button = tgui::Button::create("点击进入游戏");
        start_button->setPosition({350, 250});
        start_button->setSize({100, 100});
        start_button->setTextSize(14u);
        start_button->onPress([&] {
            m_outside_dispather->enqueue<thr::ecs::game_state_push_event>(
                std::make_unique<game_screen>());
        });
        m_global_gui->add(start_button, "start_button");
    }
    void main_menu::on_pause() noexcept {
        tgui::Button::Ptr start_button = m_global_gui->get<tgui::Button>("start_button");
        start_button->setVisible(false);
        m_is_paused = true;
    }
    void main_menu::on_resume() noexcept {
        tgui::Button::Ptr start_button = m_global_gui->get<tgui::Button>("start_button");
        start_button->setVisible(true);
        m_is_paused = false;
    }
    bool main_menu::handle_event([[maybe_unused]] const sf::Event &event) noexcept { return false; }
    void main_menu::update([[maybe_unused]] thr::ecs::milliseconds_f delta_time) noexcept {}
    void main_menu::draw() noexcept {}
    void main_menu::connect_dispatcher() noexcept {}
    void main_menu::disconnect_dispatcher() noexcept {}

    // game_screen
    game_screen::game_screen() noexcept : m_player_entity(m_registry.create()) {
        connect_dispatcher();
        nlohmann::json json;
        std::ifstream  fin("level1.json");
        fin >> json;
        thr::ecs::level_serialization_system::deserialize_from_json(m_registry, json);
        m_registry.emplace<thr::ecs::player_on_ground>(
            m_player_entity, m_registry.ctx().get<thr::ecs::level_info>().start_segment_entity);
        m_registry.emplace<thr::ecs::turning_history>(m_player_entity);
    }
    game_screen::~game_screen() noexcept {
        tgui::Button::Ptr exit_button = m_global_gui->get<tgui::Button>("exit_button");
        m_global_gui->remove(exit_button);
        disconnect_dispatcher();
    }

    void game_screen::init() noexcept {
        tgui::Button::Ptr exit_button = tgui::Button::create("退出/Esc");
        exit_button->onPress([&] { m_outside_dispather->enqueue<thr::ecs::game_state_pop_event>(); });
        m_global_gui->add(exit_button, "exit_button");
    }
    void game_screen::on_pause() noexcept { m_is_paused = true; }
    void game_screen::on_resume() noexcept { m_is_paused = false; }
    bool game_screen::handle_event(const sf::Event &event) noexcept {
        if (m_is_paused) {
            return false;
        }
        if (const auto *key_pressed = event.getIf<sf::Event::KeyPressed>()) {
            if (key_pressed->control && key_pressed->code == sf::Keyboard::Key::Z) {
                // Crtl+Z 撤销。
                thr::ecs::player_movement_system::undo(m_registry, m_player_entity);
                return true;
            }
            if (key_pressed->code == sf::Keyboard::Key::Escape) {
                m_outside_dispather->enqueue<thr::ecs::game_state_pop_event>();
                return true;
            }
        }
        return false;
    }
    void game_screen::update(thr::ecs::milliseconds_f delta_time) noexcept {
        constexpr float velocity_per_millisecond = 0.2f;

        if (m_winned_time.has_value()) {
            using namespace std::chrono_literals;
            if (thr::ecs::clock::now() - *m_winned_time >= 3s) {
                m_outside_dispather->enqueue(thr::ecs::game_state_pop_event{});
            }
            return;
        }

        if (m_window->hasFocus()) {
            float delta_length = velocity_per_millisecond * delta_time.count();
            // NOLINTNEXTLINE(readability-identifier-length)
            bool  up = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)
                       || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up);
            bool  down = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)
                         || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down);
            bool  left = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)
                         || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left);
            bool  right = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)
                          || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right);

            bool  updated = false;
            if (up) {
                if (!down) {
                    thr::ecs::player_movement_system::update(m_registry, m_player_entity, delta_length,
                                                             thr::ecs::direction::up);
                    updated = true;
                }
            } else if (down) {
                thr::ecs::player_movement_system::update(m_registry, m_player_entity, delta_length,
                                                         thr::ecs::direction::down);
                updated = true;
            }
            if (left) {
                if (!right) {
                    thr::ecs::player_movement_system::update(m_registry, m_player_entity, delta_length,
                                                             thr::ecs::direction::left);
                    updated = true;
                }
            } else if (right) {
                thr::ecs::player_movement_system::update(m_registry, m_player_entity, delta_length,
                                                         thr::ecs::direction::right);
                updated = true;
            }
            if (updated) {
                if (std::ranges::any_of(
                        m_registry.view<thr::ecs::player_on_ground>(), [&](entt::entity entity) {
                            return m_registry.get<thr::ecs::player_on_ground>(entity).segment_entity
                                   == m_registry.ctx().get<thr::ecs::level_info>().end_segment_entity;
                        })) {
                    if (std::ranges::all_of(
                            m_registry.view<thr::ecs::segment>(), [&](entt::entity entity) {
                                return thr::no_nan_inf_f{
                                           m_registry.get<thr::ecs::segment>(entity).walked_precent}
                                       == thr::no_nan_inf_f{1};
                            })) {
                        // 赢了。
                        m_winned_time = thr::ecs::clock::now();
                    }
                }
            }
        }
    }
    void game_screen::draw() noexcept {
        thr::ecs::level_render_system::draw(m_registry, *m_window);

        if (m_winned_time.has_value()) {

            std::string string = std::format("你赢了！\n{} 秒后退出",
                                             3
                                                 - std::chrono::duration_cast<std::chrono::seconds>(
                                                       thr::ecs::clock::now() - *m_winned_time)
                                                       .count());
            sf::Text    text(get_sfml_font(), sf::String::fromUtf8(string.begin(), string.end()));
            text.setLineAlignment(sf::Text::LineAlignment::Center);
            text.setPosition(m_window->getView().getCenter());
            sf::RectangleShape rect(text.getGlobalBounds().size);
            rect.setPosition(text.getGlobalBounds().position);
            rect.setFillColor(sf::Color::Black);
            m_window->draw(rect);
            m_window->draw(text);
        }
    }
    void game_screen::connect_dispatcher() noexcept {
        thr::ecs::segment::connect_listener(m_registry);
        thr::ecs::node::connect_listener(m_registry);
        thr::ecs::player_under_ground::connect_listener(m_registry);
    }
    void game_screen::disconnect_dispatcher() noexcept {
        thr::ecs::segment::disconnect_listener(m_registry);
        thr::ecs::node::disconnect_listener(m_registry);
        thr::ecs::player_under_ground::disconnect_listener(m_registry);
    }

    // pause_menu
    pause_menu::pause_menu() noexcept { connect_dispatcher(); }
    pause_menu::~pause_menu() noexcept { disconnect_dispatcher(); }

    void pause_menu::on_pause() noexcept { m_is_paused = true; }
    void pause_menu::on_resume() noexcept { m_is_paused = false; }
    bool pause_menu::handle_event([[maybe_unused]] const sf::Event &event) noexcept { return false; }
    void pause_menu::update([[maybe_unused]] thr::ecs::milliseconds_f delta_time) noexcept {}
    void pause_menu::draw() noexcept {}
    void pause_menu::connect_dispatcher() noexcept {}
    void pause_menu::disconnect_dispatcher() noexcept {}

} // namespace mainhelper
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)