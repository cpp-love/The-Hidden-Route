/**
 * @file game_base.hpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 定义了游戏 ECS 系统的基础组件。
 * @version 0.1.0-2
 * @date 2026-05-23
 * 
 * @copyright cpp-love
 * 
 * @details
 * 
 */

#ifndef THR_ECS_COMPONENTS_GLOBAL_GAME_BASE_HPP
#define THR_ECS_COMPONENTS_GLOBAL_GAME_BASE_HPP

#include "thr/base/assert_msg.hpp"
#include <SFML/System/Vector2.hpp>
#include <chrono>
#include <ratio>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace thr {

    namespace ecs {

        constexpr float block_side_length = 50.f; ///< 区块边长。

        /// @brief 方向枚举。
        enum class direction : std::uint8_t { right = 0, down, left, up };

        /**
         * @brief 获取方向名字。
         * @param [in] dir 方向。
         * @return std::string 名字。
         */
        inline std::string direction_to_name(direction dir) {
            static_assert(std::to_underlying(direction::up) == 3, "此函数对于 direction 枚举的处理不够");
            switch (dir) {
                case direction::right: return "right";
                case direction::down: return "down";
                case direction::left: return "left";
                case direction::up: return "up";
            }
            unreachable("参数 dir（值为：{}）非法", std::to_underlying(dir));
        }

        /**
         * @brief 从方向名字获取方向枚举。
         * @param [in] dir 方向名字。
         * @return direction 方向枚举。
         * @exception std::invalid_argument 当方向名字非法时输出
         */
        inline direction name_to_direction(std::string_view dir) {
            static_assert(std::to_underlying(direction::up) == 3, "此函数对于 direction 枚举的处理不够");
            if (dir == "right") {
                return direction::right;
            }
            if (dir == "down") {
                return direction::down;
            }
            if (dir == "left") {
                return direction::left;
            }
            if (dir == "up") {
                return direction::up;
            }
            throw std::invalid_argument(
                "in thr::ecs::from_direction_name(): unsupported direction name.");
        }

        /**
         * @brief 以指定长度转换到 `sf::Vector2f`。
         * @param [in] dir 方向。
         * @param [in] length 指定的长度。
         * @return sf::Vector2f 转换结果。
         */
        inline sf::Vector2f direction_to_vector2f(direction dir, float length) {
            static_assert(std::to_underlying(direction::up) == 3, "此函数对于 direction 枚举的处理不够");
            switch (dir) {
                case direction::right: return {length, 0};
                case direction::down: return {0, length};
                case direction::left: return {-length, 0};
                case direction::up: return {0, -length};
            }
            unreachable("参数 dir（值为：{}）非法", std::to_underlying(dir));
        }

        /**
         * @brief 获取相反方向。
         * @param [in] dir 方向。
         * @return direction 相反方向。
         */
        inline direction negate_direction(direction dir) {
            static_assert(std::to_underlying(direction::up) == 3, "此函数对于 direction 枚举的处理不够");
            THR_ASSERT_MSG(std::to_underlying(direction::right) <= std::to_underlying(dir)
                               && std::to_underlying(direction::up) >= std::to_underlying(dir),
                           "参数 dir（值为：{}）非法", std::to_underlying(dir));
            // 将 switch-case 表优化。
            return static_cast<direction>(std::to_underlying(dir)
                                          ^ static_cast<std::underlying_type_t<direction>>(0b10));
        }

        /**
         * @brief 获取顺时针旋转90°的方向。
         * @param [in] dir 方向。
         * @return direction 顺时针旋转90°的方向。
         */
        inline direction rotate_90_cw(direction dir) {
            static_assert(std::to_underlying(direction::up) == 3, "此函数对于 direction 枚举的处理不够");
            THR_ASSERT_MSG(std::to_underlying(direction::right) <= std::to_underlying(dir)
                               && std::to_underlying(direction::up) >= std::to_underlying(dir),
                           "参数 dir（值为：{}）非法", std::to_underlying(dir));
            // 将 switch-case 表优化。
            return static_cast<direction>(
                static_cast<std::underlying_type_t<direction>>(
                    std::to_underlying(dir) + static_cast<std::underlying_type_t<direction>>(0b1))
                & static_cast<std::underlying_type_t<direction>>(0b11));
        }

        /**
         * @brief 获取逆时针旋转90°的方向。
         * @param [in] dir 方向。
         * @return direction 逆时针旋转90°的方向。
         */
        inline direction rotate_90_ccw(direction dir) {
            static_assert(std::to_underlying(direction::up) == 3, "此函数对于 direction 枚举的处理不够");
            THR_ASSERT_MSG(std::to_underlying(direction::right) <= std::to_underlying(dir)
                               && std::to_underlying(direction::up) >= std::to_underlying(dir),
                           "参数 dir（值为：{}）非法", std::to_underlying(dir));
            // 将 switch-case 表优化。
            return static_cast<direction>(
                static_cast<std::underlying_type_t<direction>>(
                    std::to_underlying(dir) - static_cast<std::underlying_type_t<direction>>(0b1))
                & static_cast<std::underlying_type_t<direction>>(0b11));
        }

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