/**
 * @file game_state_manager.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 实现了游戏状态系统。
 * @version 0.1.0-1
 * @date 2026-05-02
 * 
 * @copyright cpp-love
 * 
 */

#include "thr/ecs/systems/global/game_state_manager.hpp"
#include "thr/base/assert_msg.hpp"
#include "thr/ecs/components/global/game_state_components.hpp"
#include <entt/entity/entity.hpp>
#include <memory>
#include <ranges>
#include <spdlog/spdlog.h>

namespace thr::ecs {

    game_state_manager::game_state_manager() noexcept {
        m_dispatcher.sink<game_state_push_event>().connect<&game_state_manager::on_push_state>(this);
        m_dispatcher.sink<game_state_pop_event>().connect<&game_state_manager::on_pop_state>(this);
    }

    void game_state_manager::push_state(game_state_base::ptr state) noexcept {
        state->bind(m_dispatcher);
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
        m_dispatcher.clear();
        m_dispatcher.sink<game_state_push_event>().connect<&game_state_manager::on_push_state>(this);
        m_dispatcher.sink<game_state_pop_event>().connect<&game_state_manager::on_pop_state>(this);
    }
    bool game_state_manager::handle_event(const sf::Event &event) noexcept {
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
        m_dispatcher.update();
    }
    void game_state_manager::draw(sf::RenderTarget &render) noexcept {
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
        for (auto &state : last_unblocked_range) { state->draw(render); }
    }

    void game_state_manager::on_push_state(game_state_push_event &event) noexcept {
        push_state(std::move(event.state));
    }
    void game_state_manager::on_pop_state([[maybe_unused]] game_state_pop_event event) noexcept {
        pop_state();
    }

} // namespace thr::ecs
