/**
 * @file game_state_manager.cpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 实现了游戏状态系统。
 * @version 0.1.0-5
 * @date 2026-07-14
 * 
 * @copyright cpp-love
 * 
 */

#include "thr/ecs/systems/global/game_state_manager.hpp"
#include "thr/base/assert_msg.hpp"
#include "thr/ecs/components/global/game_state_components.hpp"
#include "thr/ecs/configs.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <TGUI/Container.hpp>
#include <TGUI/Layout.hpp>
#include <TGUI/Rect.hpp>
#include <TGUI/RelFloatRect.hpp>
#include <TGUI/String.hpp>
#include <TGUI/Widgets/Panel.hpp>
#include <entt/entity/entity.hpp>
#include <memory>
#include <ranges>
#include <spdlog/spdlog.h>

namespace thr::ecs {

    const tgui::String game_state_manager::game_screen_panel_name{"game_screen_panel"};

    game_state_manager::game_state_manager(sf::RenderWindow &window) noexcept
        : m_window(window), m_gui(m_window) {
        m_dispatcher.sink<game_state_push_event>().connect<&game_state_manager::on_push_state>(this);
        m_dispatcher.sink<game_state_pop_event>().connect<&game_state_manager::on_pop_state>(this);
        m_gui.setFont(thr::ecs::configs::singleton().get_tgui_font());
        m_gui.setTextSize(14u);
        tgui::Panel::Ptr game_screen_panel = tgui::Panel::create();
        game_screen_panel->getRenderer()->setBackgroundColor(tgui::Color::Transparent);
        m_gui.add(game_screen_panel, game_screen_panel_name);
    }

    game_state_manager::~game_state_manager() noexcept {
        m_states.clear();
        m_dispatcher.clear();
    }

    void game_state_manager::process_pending_states() noexcept {
        while (!m_pending_states.empty()) {
            auto state = std::move(m_pending_states.back());
            m_pending_states.pop_back();
            state->bind(m_dispatcher, m_window, m_gui);
            if (!m_states.empty()) {
                m_states.back()->on_pause();
            }
            m_states.push_back(std::move(state));
        }
    }

    void game_state_manager::push_state(game_state_base::ptr state) noexcept {
        if (m_is_dispatching) {
            m_pending_states.push_back(std::move(state));
            return;
        }

        state->bind(m_dispatcher, m_window, m_gui);
        if (!m_states.empty()) {
            m_states.back()->on_pause();
        }
        m_states.push_back(std::move(state));
    }
    void game_state_manager::pop_state() noexcept {
        THR_ASSERT_MSG(!m_states.empty(), "用户错误地尝试对空的栈执行弹出操作");
        m_states.pop_back();
        if (!m_states.empty()) {
            m_states.back()->on_resume();
        }
    }
    void game_state_manager::reset() noexcept {
        m_states.clear();
        m_pending_states.clear();
        m_is_dispatching = false;
        m_dispatcher.clear();
        m_dispatcher.sink<game_state_push_event>().connect<&game_state_manager::on_push_state>(this);
        m_dispatcher.sink<game_state_pop_event>().connect<&game_state_manager::on_pop_state>(this);
    }
    bool game_state_manager::handle_event(const sf::Event &event) noexcept {
        if (event.is<sf::Event::Closed>()) {
            spdlog::info("Quit the window.");
            m_window.close();
            return true;
        }

        if (const auto *resized = event.getIf<sf::Event::Resized>()) {
            // 更新视图。
            float window_ratio =
                static_cast<float>(resized->size.x) / static_cast<float>(resized->size.y);
            sf::View view{m_window.getView()};
            float    old_view_ratio =
                static_cast<float>(view.getSize().x) / static_cast<float>(view.getSize().y);
            sf::FloatRect    viewport{{0, 0}, {1, 1}};
            tgui::Panel::Ptr game_screen_panel =
                m_gui.get<tgui::Panel>(thr::ecs::game_state_manager::game_screen_panel_name);

            // 根据长宽比计算视口位置和大小，确保黑边方向正确。
            if (window_ratio > old_view_ratio) {
                // 窗口更扁，黑边在左右。
                viewport.size.x = old_view_ratio / window_ratio;
                viewport.position.x = (1 - viewport.size.x) / 2;
                game_screen_panel->setPosition({viewport.position.x * resized->size.x, 0});
                game_screen_panel->setSize({viewport.size.x * resized->size.x, resized->size.y});
            } else {
                // 窗口更瘦高，黑边在上下。
                viewport.size.y = window_ratio / old_view_ratio;
                viewport.position.y = (1 - viewport.size.y) / 2;
                game_screen_panel->setPosition({0, viewport.position.y * resized->size.y});
                game_screen_panel->setSize({resized->size.x, viewport.size.y * resized->size.y});
            }

            // 创建并应用新的视口。
            view.setViewport(viewport);
            m_window.setView(view);
            m_gui.setAbsoluteView(
                {0, 0, static_cast<float>(resized->size.x), static_cast<float>(resized->size.y)});
            return true;
        }

        if (m_gui.handleEvent(event)) {
            return true;
        }

        for (auto &state : m_states | std::views::reverse) {
            if (state->handle_event(event)) {
                return true;
            }
            if (state->should_block_passing_down()) {
                break;
            }
        }
        return false;
    }
    void game_state_manager::update(milliseconds_f delta_time) noexcept {
        for (auto &state : m_states | std::views::reverse) {
            state->update(delta_time);
            if (state->should_block_passing_down()) {
                break;
            }
        }

        m_is_dispatching = true;
        m_dispatcher.update();
        m_is_dispatching = false;
        process_pending_states();
    }
    void game_state_manager::draw() noexcept {
        /// @todo 添加更好的方式
        // 后面覆盖前面，所以前面先绘制
        std::ranges::subrange last_unblocked_range{[&] {
                                                       for (auto it = m_states.rbegin();
                                                            it != m_states.rend(); ++it) {
                                                           if ((*it)->should_block_passing_down()) {
                                                               return std::prev(it.base());
                                                           }
                                                       }
                                                       return m_states.begin();
                                                   }(),
                                                   m_states.end()};
        for (auto &state : last_unblocked_range) { state->draw(); }
        m_gui.draw();
    }

    void game_state_manager::on_push_state(game_state_push_event &event) noexcept {
        push_state(std::move(event.state));
    }
    void game_state_manager::on_pop_state([[maybe_unused]] game_state_pop_event event) noexcept {
        pop_state();
    }

} // namespace thr::ecs
