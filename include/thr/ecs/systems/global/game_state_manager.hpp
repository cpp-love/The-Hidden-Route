/**
 * @file game_state_manager.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 声明了游戏状态管理系统。
 * @version 0.1.0-2
 * @date 2026-06-19
 * 
 * @copyright cpp-love
 * 
 */

#ifndef THR_ECS_SYSTEMS_GLOBAL_GAME_STATE_MANAGER_HPP
#define THR_ECS_SYSTEMS_GLOBAL_GAME_STATE_MANAGER_HPP

#include "thr/ecs/components/global/game_state_components.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <entt/signal/dispatcher.hpp>
#include <vector>

namespace thr::ecs {

    /// @brief 压入状态的事件组件。
    struct game_state_push_event final {
        game_state_base::ptr state; ///< 游戏状态。
    };
    /// @brief 弹出状态的事件组件。
    struct game_state_pop_event final {};

    /// @brief 游戏状态管理系统。
    class game_state_manager {
      private:
        std::vector<game_state_base::ptr> m_states;     ///< 状态栈。
        entt::dispatcher                  m_dispatcher; ///< 调度器。
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
        sf::RenderWindow                 &m_window; ///< 渲染窗口引用。
        tgui::Gui                         m_gui;    ///< GUI 对象。

      public:
        /**
         * @brief 构建一个 game state manager 对象。
         * @param [in] window 渲染窗口。
         */
        explicit game_state_manager(sf::RenderWindow &window) noexcept;

        /// @brief 销毁 game state manager 对象。
        ~game_state_manager() noexcept;

        /**
         * @brief 压入状态。
         * @param [in] state 游戏状态。
         * @note 在函数内部会给 `state` 绑定调度器，无需提前绑定。
         */
        void push_state(game_state_base::ptr state) noexcept;
        /// @brief 弹出状态。
        void pop_state() noexcept;
        /// @brief 重启对象。
        void reset() noexcept;
        /**
         * @brief 处理事件。
         * @param [in] event 事件。
         * @return true 事件已处理。
         * @return false 事件未处理。
         */
        bool handle_event(const sf::Event &event) noexcept;
        /**
         * @brief 更新游戏状态。
         * @param [in] delta_time 时间间隔。
         */
        void update(milliseconds_f delta_time) noexcept;
        /// @brief 绘制当前状态。
        void draw() noexcept;

      private:
        /**
         * @brief 响应压入状态的函数。
         * @param [in] event 压入状态。
         */
        void on_push_state(game_state_push_event &event) noexcept;
        /**
         * @brief 响应弹出状态的函数。
         * @param [in] event 弹出状态。
         */
        void on_pop_state([[maybe_unused]] game_state_pop_event event) noexcept;
    };

} // namespace thr::ecs

#endif // THR_ECS_SYSTEMS_GLOBAL_GAME_STATE_MANAGER_HPP