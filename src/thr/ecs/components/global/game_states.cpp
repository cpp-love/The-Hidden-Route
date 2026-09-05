/**
 * @file game_states.cpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 实现了一些具体的游戏状态。
 * @version 0.1.0-8
 * @date 2026-07-22
 * 
 * @copyright cpp-love
 * 
 * @details 暂时使用思源宋体作为字体。
 */

#include "thr/base/assert_msg.hpp"
#include "thr/base/file.hpp"
#include "thr/base/floating_point_compare.hpp"
#include "thr/ecs.hpp"
#include "thr/ecs/components/maze_components.hpp"
#include "thr/undo.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <algorithm>
#include <chrono>
#include <entt/signal/dispatcher.hpp>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <sol/sol.hpp>
#include <spdlog/spdlog.h>
#include <string_view>
#include <variant>

namespace mainhelper {

    // settings_menu
    settings_menu::settings_menu() { connect_dispatcher(); }
    settings_menu::~settings_menu() { disconnect_dispatcher(); }

    void settings_menu::init() noexcept {}
    void settings_menu::on_pause() noexcept { m_is_paused = true; }
    void settings_menu::on_resume() noexcept { m_is_paused = false; }
    bool settings_menu::handle_event([[maybe_unused]] const sf::Event &event) noexcept { return false; }
    void settings_menu::update([[maybe_unused]] thr::ecs::milliseconds_f delta_time) noexcept {}
    void settings_menu::draw() noexcept {}
    void settings_menu::connect_dispatcher() noexcept {}
    void settings_menu::disconnect_dispatcher() noexcept {}

    // main_menu
    main_menu::main_menu() {
        connect_dispatcher();
        thr::ecs::lua_bindings::bind_to_lua(m_lua, m_registry);
        auto script_path = thr::get_existing_full_path("assets/lua_scripts/main_menu.lua").value();
        if (auto result = m_lua.safe_script_file(script_path.generic_string()); !result.valid()) {
            spdlog::warn("Failed to load and run script file {}: {}", script_path.generic_string(),
                         result.get<sol::error>().what());
        }
    }
    main_menu::~main_menu() {
        tgui::Button::Ptr start_button = m_global_gui->get<tgui::Button>("start_button");
        m_global_gui->remove(start_button);
        disconnect_dispatcher();
    }

    void main_menu::init() {
        tgui::Button::Ptr start_button = tgui::Button::create("点击进入游戏");
        start_button->setPosition({"(&.size - size) / 2", "(&.size - size) / 2"});
        start_button->setSize({"10%", "10%"});
        start_button->onPress([&] {
            m_outside_dispather->enqueue<thr::ecs::game_state_push_event>(
                std::make_unique<level_graph_screen>());
        });
        start_button->getRenderer()->setRoundedBorderRadius(10.f);
        m_global_gui->add(start_button, "start_button");
    }
    void main_menu::on_pause() {
        m_global_gui->get("start_button")->setVisible(false);
        m_is_paused = true;
    }
    void main_menu::on_resume() {
        m_global_gui->get("start_button")->setVisible(true);
        m_is_paused = false;
    }
    bool main_menu::handle_event([[maybe_unused]] const sf::Event &event) noexcept { return false; }
    void main_menu::update(thr::ecs::milliseconds_f delta_time) {
        auto *scheduler = m_registry.ctx().find<thr::ecs::scheduler>();
        if (scheduler != nullptr) {
            scheduler->update(delta_time);
        }
    }
    void main_menu::draw() {
        // draw texts
        auto texts = m_registry.view<sf::Text>();
        for (const auto &[entity, text] : texts.each()) {
            m_window->draw(text);
        }
    }
    void main_menu::connect_dispatcher() noexcept {}
    void main_menu::disconnect_dispatcher() noexcept {}

    // level_graph_screen
    level_graph_screen::level_graph_screen() {
        thr::ecs::lua_bindings::bind_to_lua(m_lua, m_registry);
        // adapted from main_helper::game_screen::game_screen
        nlohmann::json json;
        std::ifstream  fin(thr::get_existing_full_path("assets/json/level_graph.json")
                               .value() /*实在不行就抛异常爆炸*/);
        fin >> json;
        thr::ecs::level_graph_serialization_system::deserialize_from_json(m_registry, json);
        auto script_path =
            thr::get_existing_full_path("assets/lua_scripts/level_graph_screen.lua").value();
        if (auto result = m_lua.safe_script_file(script_path.generic_string()); !result.valid()) {
            spdlog::warn("Failed to load and run script file {}: {}", script_path.generic_string(),
                         result.get<sol::error>().what());
        }
    }
    level_graph_screen::~level_graph_screen() {
        // adapted from thr::ecs::<thr/ecs/systems/level_graph_render_system.cpp's private namespace>::remove_existing_nodes
        // 清空按钮。
        tgui::Panel::Ptr panel =
            m_global_gui->get<tgui::Panel>(thr::ecs::game_state_manager::game_screen_panel_name);
        auto widgets = panel->getWidgets();
        for (const auto &widget : widgets) {
            if (widget->getWidgetName().starts_with(
                    tgui::String(thr::ecs::level_graph_render_system::widget_prefix))) {
                panel->remove(widget);
            }
        }
        m_global_gui->remove(m_global_gui->get("level_graph_screen_exit_button"));
        disconnect_dispatcher();
        nlohmann::json json = thr::ecs::level_graph_serialization_system::serialize_to_json(m_registry);
        std::ofstream  fout(thr::get_existing_full_path("assets/json/level_graph.json")
                                .value() /*实在不行就抛异常爆炸*/);
        fout << json;
    }

    void level_graph_screen::init() {
        connect_dispatcher();
        tgui::Button::Ptr exit_button = tgui::Button::create("退出/Esc");
        exit_button->setSize({"5%", "5%"});
        exit_button->onPress([&] { m_outside_dispather->enqueue<thr::ecs::game_state_pop_event>(); });
        exit_button->getRenderer()->setRoundedBorderRadius(10.f);
        m_global_gui->add(exit_button, "level_graph_screen_exit_button");
    }
    void level_graph_screen::on_pause() {
        auto widgets =
            m_global_gui->get<tgui::Panel>(thr::ecs::game_state_manager::game_screen_panel_name)
                ->getWidgets();
        for (const auto &widget : widgets) {
            if (widget->getWidgetName().starts_with(
                    tgui::String(thr::ecs::level_graph_render_system::widget_prefix))) {
                widget->setVisible(false);
            }
        }
        m_global_gui->get("level_graph_screen_exit_button")->setVisible(false);
        m_is_paused = true;
    }
    void level_graph_screen::on_resume() {
        auto widgets =
            m_global_gui->get<tgui::Panel>(thr::ecs::game_state_manager::game_screen_panel_name)
                ->getWidgets();
        for (const auto &widget : widgets) {
            if (widget->getWidgetName().starts_with(
                    tgui::String(thr::ecs::level_graph_render_system::widget_prefix))) {
                widget->setVisible(true);
            }
        }
        m_global_gui->get("level_graph_screen_exit_button")->setVisible(true);
        m_is_paused = false;
    }
    bool level_graph_screen::handle_event(const sf::Event &event) {
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
    void level_graph_screen::update(thr::ecs::milliseconds_f delta_time) {
        auto *scheduler = m_registry.ctx().find<thr::ecs::scheduler>();
        if (scheduler != nullptr) {
            scheduler->update(delta_time);
        }
    }
    void level_graph_screen::draw() {
        thr::ecs::level_graph_render_system::draw(
            m_registry,
            m_global_gui->get<tgui::Panel>(thr::ecs::game_state_manager::game_screen_panel_name),
            *m_window, m_registry.ctx().get<thr::ecs::start_level>().entity, [&](entt::entity entity) {
                const auto &node = m_registry.get<thr::ecs::level_node>(entity);
                spdlog::info("进入关卡 {}。", node.name);
                THR_ASSERT_MSG(!node.locked, "节点不应未解锁。");
                m_current_level_entity = entity;
                m_outside_dispather->enqueue<thr::ecs::game_state_push_event>(
                    std::make_unique<game_screen>(node.name));
            });
        // draw texts
        auto texts = m_registry.view<sf::Text>();
        for (const auto &[entity, text] : texts.each()) {
            m_window->draw(text);
        }
    }
    void level_graph_screen::on_level_finished(const level_finished_event &event) {
        const auto &node = m_registry.get<thr::ecs::level_node>(m_current_level_entity);
        THR_ASSERT_MSG(!node.locked, "节点不应未解锁。");
        spdlog::info("关卡 {} 完成。", node.name);
        for (entt::entity entity : node.relative_entities) {
            auto &next_node = m_registry.get<thr::ecs::level_node>(entity);
            if ((!next_node.hidden && event.should_unlock_relative_levels)
                || std::ranges::contains(event.extra_levels, next_node.name)) {
                next_node.locked = false;
            }
        }
    }
    void level_graph_screen::connect_dispatcher() {
        m_outside_dispather->sink<level_finished_event>()
            .connect<&level_graph_screen::on_level_finished>(this);
    }
    void level_graph_screen::disconnect_dispatcher() {
        m_outside_dispather->sink<level_finished_event>()
            .disconnect<&level_graph_screen::on_level_finished>(this);
    }

    // game_screen
    game_screen::game_screen(std::string_view level_name) : m_player_entity(m_registry.create()) {
        connect_dispatcher();
        nlohmann::json json;
        // adapted from thr::ecs::configs::singleton
        std::ifstream  fin(thr::get_existing_full_path(std::format("assets/json/{}.json", level_name))
                               .value() /*实在不行就抛异常爆炸*/);
        fin >> json;
        thr::ecs::level_serialization_system::deserialize_from_json(m_registry, json);
        entt::entity start_segment_entity =
            m_registry.ctx().get<thr::ecs::level_info>().start_segment_entity;
        thr::ecs::player player{.color = sf::Color{40, 40, 170},
                                .statuses{{thr::ecs::player::on_ground{start_segment_entity}}}};
        m_registry.emplace<thr::ecs::player>(m_player_entity, std::move(player));
        m_registry.patch<thr::ecs::segment>(start_segment_entity, [&](thr::ecs::segment &seg) {
            seg.infos.get_current_state_modifiable().current_walking_entity = m_player_entity;
        });
        if (const auto *script = m_registry.ctx().find<thr::ecs::level_script>()) {
            m_lua = sol::state{};
            thr::ecs::lua_bindings::bind_to_lua(*m_lua, m_registry);
            if (auto result = m_lua->safe_script_file(script->script_path.generic_string());
                !result.valid()) {
                spdlog::warn("Failed to load and run script file {}: {}",
                             script->script_path.generic_string(), result.get<sol::error>().what());
            }
            if (sol::protected_function function = (*m_lua)["on_level_started"]; function.valid()) {
                if (auto result = function(); !result.valid()) {
                    spdlog::warn("Failed to call to lua function on_level_started: {}",
                                 result.get<sol::error>().what());
                }
            }
        }
    }
    game_screen::~game_screen() {
        tgui::Button::Ptr exit_button = m_global_gui->get<tgui::Button>("exit_button");
        m_global_gui->remove(exit_button);
        disconnect_dispatcher();
    }

    void game_screen::init() {
        tgui::Button::Ptr exit_button = tgui::Button::create("退出/Esc");
        exit_button->setSize({"5%", "5%"});
        exit_button->onPress([&] { m_outside_dispather->enqueue<thr::ecs::game_state_pop_event>(); });
        exit_button->getRenderer()->setRoundedBorderRadius(10.f);
        m_global_gui->add(exit_button, "exit_button");
    }
    void game_screen::on_pause() noexcept { m_is_paused = true; }
    void game_screen::on_resume() noexcept { m_is_paused = false; }
    bool game_screen::handle_event(const sf::Event &event) {
        if (m_is_paused) {
            return false;
        }
        if (const auto *key_pressed = event.getIf<sf::Event::KeyPressed>()) {
            if (key_pressed->control && key_pressed->code == sf::Keyboard::Key::Z) {
                m_undo_manager.undo();
                m_previous_direction.switch_to_previous();
                return true;
            }
            if (key_pressed->control && key_pressed->code == sf::Keyboard::Key::Y) {
                m_undo_manager.redo();
                m_previous_direction.switch_to_next();
                return true;
            }
            if (key_pressed->code == sf::Keyboard::Key::Escape) {
                m_outside_dispather->enqueue<thr::ecs::game_state_pop_event>();
                return true;
            }
        }
        return false;
    }

    void game_screen::update(thr::ecs::milliseconds_f delta_time) {
        // update scheduler
        auto *scheduler = m_registry.ctx().find<thr::ecs::scheduler>();
        if (scheduler != nullptr) {
            scheduler->update(delta_time);
        }

        // update remaining time
        if (m_remaining_time_after_winning.has_value()) {
            using namespace std::chrono_literals;
            *m_remaining_time_after_winning -= delta_time;
            if (*m_remaining_time_after_winning < 0s) {
                m_outside_dispather->enqueue(thr::ecs::game_state_pop_event{});
            }
            return;
        }

        if (!m_window->hasFocus()) {
            // 窗口没有焦点，跳过。
            return;
        }

        thr::ecs::combined_direction cdir = thr::ecs::combined_direction::none; //< 移动方向。
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)
            || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
            cdir |= thr::ecs::combined_direction::up;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)
            || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
            cdir |= thr::ecs::combined_direction::down;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)
            || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
            cdir |= thr::ecs::combined_direction::left;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)
            || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
            cdir |= thr::ecs::combined_direction::right;
        }

        if (cdir == thr::ecs::combined_direction::none) {
            // 无需更新。
            return;
        }

        if (cdir != m_previous_direction.get_current_state()) {
            // 换新方向了。
            m_undo_manager.push(
                std::make_unique<thr::undo::player_move_command>(m_registry, m_player_entity, cdir));
            m_previous_direction.get_current_state_modifiable() = cdir;
        }
        m_undo_manager.update(delta_time);

        // 若走过有特殊标签的实体，触发 Lua 脚本。
        if (m_lua.has_value()) {
            for (const auto &[entity, player] : m_registry.view<thr::ecs::player>().each()) {
                const auto &status = player.statuses.get_current_state();
                if (!std::holds_alternative<thr::ecs::player::on_ground>(status)) {
                    continue;
                }
                const auto &on_ground = std::get<thr::ecs::player::on_ground>(status);
                const auto *cur_tag = m_registry.try_get<thr::ecs::tag>(on_ground.segment_entity);
                if (cur_tag == nullptr || cur_tag->tag_ids.empty()) {
                    // 走过的实体没有特殊标签。
                    continue;
                }
                // 走过的实体有特殊标签。
                if (sol::protected_function function = (*m_lua)["on_special_segment_walked"];
                    function.valid()) {
                    if (auto result = function(thr::ecs::lua_bindings::entity_wrapper{
                            m_registry, on_ground.segment_entity});
                        !result.valid()) {
                        spdlog::warn("Failed to call to lua function on_special_segment_walked: {}",
                                     result.get<sol::error>().what());
                    }
                }
            }
        }

        // 判断是否胜利。
        if (!(std::ranges::any_of(
                  m_registry.view<thr::ecs::player>().each(),
                  [&](std::pair<entt::entity, const thr::ecs::player &> pair) {
                      const auto &status = pair.second.statuses.get_current_state();
                      return std::holds_alternative<thr::ecs::player::on_ground>(status)
                             && std::get<thr::ecs::player::on_ground>(status).segment_entity
                                    == m_registry.ctx().get<thr::ecs::level_info>().end_segment_entity;
                  }) /* 是否走到终点段 */
              && std::ranges::all_of(
                  m_registry.view<thr::ecs::segment>(),
                  [&](entt::entity entity) {
                      const auto &info =
                          m_registry.get<thr::ecs::segment>(entity).infos.get_current_state();
                      return info.prev_completed_entity.has_value()
                             || thr::no_nan_inf_f{info.walked_precent} == thr::no_nan_inf_f{1};
                  }) /* 是否所有段都走完 */
              )) {
            // 没赢。
            return;
        }

        // 赢了。
        using namespace std::chrono_literals;
        m_remaining_time_after_winning = 3s;

        level_finished_event event; //< 关卡结束时间。
        if (m_lua.has_value()) {
            if (sol::protected_function function = (*m_lua)["on_level_finished"]; function.valid()) {
                if (auto result = function(); !result.valid()) {
                    spdlog::warn("Failed to call to lua function on_level_finished: {}",
                                 result.get<sol::error>().what());
                    // 脚本运行错误，关卡结束事件回退到默认值。
                } else {
                    // 脚本运行成功，使用脚本返回的关卡结束事件。
                    sol::variadic_results res = std::move(result);
                    for (auto &value : res) {
                        if (auto opt = value.as<std::optional<bool>>()) {
                            event.should_unlock_relative_levels = *opt;
                        } else if (auto opt = value.as<std::optional<std::vector<std::string>>>()) {
                            event.extra_levels = std::move(*opt);
                        }
                    }
                }
            }
        }

        m_outside_dispather->enqueue(event);
    }

    void game_screen::draw() {
        thr::ecs::level_render_system::draw(m_registry, *m_window);

        if (m_remaining_time_after_winning.has_value()) {
            std::string string = std::format(
                "你赢了！\n{} 秒后退出",
                std::chrono::duration_cast<std::chrono::seconds>(*m_remaining_time_after_winning)
                    .count());
            sf::Text text(thr::ecs::configs::singleton().get_sfml_font(),
                          sf::String::fromUtf8(string.begin(), string.end()));
            text.setLineAlignment(sf::Text::LineAlignment::Center);
            text.setPosition(m_window->getView().getCenter());
            sf::RectangleShape rect(text.getGlobalBounds().size);
            rect.setPosition(text.getGlobalBounds().position);
            rect.setFillColor(sf::Color::Black);
            m_window->draw(rect);
            m_window->draw(text);
        }
    }
    void game_screen::connect_dispatcher() {
        thr::ecs::segment::connect_listener(m_registry);
        thr::ecs::node::connect_listener(m_registry);
        thr::ecs::player::connect_listener(m_registry);
    }
    void game_screen::disconnect_dispatcher() {
        thr::ecs::segment::disconnect_listener(m_registry);
        thr::ecs::node::disconnect_listener(m_registry);
        thr::ecs::player::disconnect_listener(m_registry);
    }

    // pause_menu
    pause_menu::pause_menu() { connect_dispatcher(); }
    pause_menu::~pause_menu() { disconnect_dispatcher(); }

    void pause_menu::init() noexcept {}
    void pause_menu::on_pause() noexcept { m_is_paused = true; }
    void pause_menu::on_resume() noexcept { m_is_paused = false; }
    bool pause_menu::handle_event([[maybe_unused]] const sf::Event &event) noexcept { return false; }
    void pause_menu::update([[maybe_unused]] thr::ecs::milliseconds_f delta_time) noexcept {}
    void pause_menu::draw() noexcept {}
    void pause_menu::connect_dispatcher() noexcept {}
    void pause_menu::disconnect_dispatcher() noexcept {}

} // namespace mainhelper