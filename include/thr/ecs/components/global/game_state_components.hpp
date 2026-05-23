/**
 * @file game_state_components.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 声明了游戏状态的组件。
 * @version 0.1.0-2
 * @date 2026-02-20
 * 
 * @copyright cpp-love
 * 
 * @details 状态采用多态的方式实现。
 * 
 */

#ifndef THR_ECS_COMPONENTS_GLOBAL_GAME_STATE_COMPONENTS_HPP
#define THR_ECS_COMPONENTS_GLOBAL_GAME_STATE_COMPONENTS_HPP

#include "thr/ecs/components/global/game_base.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <entt/fwd.hpp>
#include <memory>

namespace thr::ecs {

    /// @brief 游戏状态基类
    class game_state_base {
      public:
        using ptr = std::unique_ptr<game_state_base>;

        /// @brief 构建一个 game_state_base 对象
        game_state_base() noexcept = default;
        /// @brief 删除复制构造函数
        game_state_base(const game_state_base &rhs) noexcept = delete;
        /**
         * @brief 移动构造函数
         * @param [in] rhs 另一个游戏状态
         */
        game_state_base(game_state_base &&rhs) noexcept = default;
        /// @brief 删除复制赋值运算符重载
        game_state_base &operator=(const game_state_base &rhs) noexcept = delete;
        /// @brief 删除移动赋值运算符重载
        game_state_base &operator=(game_state_base &&rhs) noexcept = delete;
        /// @brief 虚析构函数
        virtual ~game_state_base() noexcept = default;

        /**
         * @brief 绑定调度器
         * @param [in] dispatcher 调度器
         */
        void         bind(entt::dispatcher &dispatcher) noexcept { m_outside_dispather = &dispatcher; }

        /// @brief 将此游戏状态暂停
        virtual void on_pause() noexcept = 0;
        /// @brief 将此游戏状态从暂停中恢复
        virtual void on_resume() noexcept = 0;
        /**
         * @brief 处理事件
         * @param [in] event 事件
         * @return true 事件已处理
         * @return false 事件未处理
         */
        virtual bool handle_event(const sf::Event &event) noexcept = 0;
        /**
         * @brief 更新游戏状态
         * @param [in] delta_time 时间间隔
         */
        virtual void update(milliseconds_f delta_time) noexcept = 0;
        /**
         * @brief 绘制当前状态
         * @param [in] render 需要渲染的地方
         */
        virtual void draw(sf::RenderTarget &render) noexcept = 0;
        /**
         * @brief 判断是否要阻隔绘制、更新、事件往下传递
         * @return true 要阻隔
         * @return false 不要阻隔
         * @details 默认为 true
         */
        [[nodiscard]] virtual bool should_block_passing_down() noexcept { return true; }

        /// @cond INTERNAL
      protected:
        // NOLINTNEXTLINE(cppcoreguidelines-non-private-member-variables-in-classes)
        entt::dispatcher *m_outside_dispather{}; ///< 调度器指针
        /// @endcond
    };

} // namespace thr::ecs

#endif // THR_ECS_COMPONENTS_GLOBAL_GAME_STATE_COMPONENTS_HPP