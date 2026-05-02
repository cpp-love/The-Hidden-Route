/**
 * @file game_base.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 定义了游戏 ECS 系统的基础组件。
 * @version 0.1.0-1
 * @date 2026-05-02
 * 
 * @copyright cpp-love
 * 
 * @details
 * 
 */

#ifndef THR_ECS_COMPONENTS_GLOBAL_GAME_BASE_HPP
#define THR_ECS_COMPONENTS_GLOBAL_GAME_BASE_HPP

#include <chrono>
#include <ratio>

namespace thr {

    namespace ecs {

        using clock = std::chrono::steady_clock;                         ///< 时钟类型
        using milliseconds_f = std::chrono::duration<float, std::milli>; ///< 时间间隔类型，以毫秒为单位

    } // namespace ecs

    inline namespace literals {

        inline namespace game_base_literals {

            /**
             * @brief 创建一个以毫秒为单位的时间间隔
             * @param [in] milliseconds 毫秒数
             * @return ecs::milliseconds_f 时间间隔
             */
            [[nodiscard]] constexpr ecs::milliseconds_f operator""_ms_f(long double milliseconds) {
                return ecs::milliseconds_f{static_cast<float>(milliseconds)};
            }

        } // namespace game_base_literals

    } // namespace literals

} // namespace thr

#endif // THR_ECS_COMPONENTS_GLOBAL_GAME_BASE_HPP