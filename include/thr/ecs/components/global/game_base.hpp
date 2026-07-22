/**
 * @file game_base.hpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 定义了游戏 ECS 系统的基础组件。
 * @version 0.1.0-3
 * @date 2026-07-22
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
#include <bit>
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
        enum class direction : std::uint8_t {
            right = 0u, ///< 右。
            down,       ///< 下。
            left,       ///< 左。
            up          ///< 上。
        };

        /**
         * @brief 获取方向名字。
         * @param [in] dir 方向。
         * @return std::string_view 名字。
         */
        constexpr std::string_view direction_to_name(direction dir) {
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
        constexpr direction name_to_direction(std::string_view dir) {
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
        constexpr sf::Vector2f direction_to_vector2f(direction dir, float length) {
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
        constexpr direction negate_direction(direction dir) {
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
        constexpr direction rotate_90_cw(direction dir) {
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
        constexpr direction rotate_90_ccw(direction dir) {
            THR_ASSERT_MSG(std::to_underlying(direction::right) <= std::to_underlying(dir)
                               && std::to_underlying(direction::up) >= std::to_underlying(dir),
                           "参数 dir（值为：{}）非法", std::to_underlying(dir));
            // 将 switch-case 表优化。
            return static_cast<direction>(
                static_cast<std::underlying_type_t<direction>>(
                    std::to_underlying(dir) - static_cast<std::underlying_type_t<direction>>(0b1))
                & static_cast<std::underlying_type_t<direction>>(0b11));
        }

        /**
         * @brief 判断方向是否为水平方向。
         * @param [in] dir 方向。
         * @return true 是水平方向。
         * @return false 不是水平方向。
         */
        constexpr bool is_horizontal(direction dir) {
            THR_ASSERT_MSG(std::to_underlying(direction::right) <= std::to_underlying(dir)
                               && std::to_underlying(direction::up) >= std::to_underlying(dir),
                           "参数 dir（值为：{}）非法", std::to_underlying(dir));
            // 将 switch-case 表优化。
            return (std::to_underlying(dir) & static_cast<std::underlying_type_t<direction>>(0b1)) == 0;
        }

        /**
         * @brief 判断方向是否为垂直方向。
         * @param [in] dir 方向。
         * @return true 是垂直方向。
         * @return false 不是垂直方向。
         */
        constexpr bool is_vertical(direction dir) {
            THR_ASSERT_MSG(std::to_underlying(direction::right) <= std::to_underlying(dir)
                               && std::to_underlying(direction::up) >= std::to_underlying(dir),
                           "参数 dir（值为：{}）非法", std::to_underlying(dir));
            // 将 switch-case 表优化。
            return (std::to_underlying(dir) & static_cast<std::underlying_type_t<direction>>(0b1)) != 0;
        }

        /// @brief 综合方向枚举。
        enum class combined_direction : std::uint8_t {
            none = 0u,        ///< 没有方向。
            right = 1u << 0u, ///< 右。
            down = 1u << 1u,  ///< 下。
            left = 1u << 2u,  ///< 左。
            up = 1u << 3u     ///< 上。
        };

        /**
         * @brief 重载或运算，用于组合方向。
         * @param [in] lhs 方向1。
         * @param [in] rhs 方向2。
         * @return combined_direction 组合后的方向。
         * @note 此函数只会保留至多两种方向，如果出现 `left | right` 或 `up | down` 这种情况会自动抵消。
         */
        constexpr combined_direction operator|(combined_direction lhs, combined_direction rhs) noexcept {
            using underlying_t = std::underlying_type_t<combined_direction>;
            underlying_t           val = std::to_underlying(lhs) | std::to_underlying(rhs);

            constexpr underlying_t h_mask =
                std::to_underlying(combined_direction::left)
                | std::to_underlying(combined_direction::right); //< 水平方向掩码。
            constexpr underlying_t v_mask =
                std::to_underlying(combined_direction::up)
                | std::to_underlying(combined_direction::down); //< 垂直方向掩码。

            underlying_t horizontal = val & h_mask;
            underlying_t vertical = val & v_mask;

            // 左右同时存在，归零。
            if (horizontal == h_mask) {
                horizontal = 0;
            }
            // 上下同时存在，归零
            if (vertical == h_mask) {
                vertical = 0;
            }

            return static_cast<combined_direction>(horizontal | vertical);
        }

        /**
         * @brief 重载或赋值运算，用于组合方向。
         * @param [in, out] lhs 方向1。
         * @param [in] rhs 方向2。
         * @return combined_direction& 组合后的方向。
         * @note 此函数只会保留至多两种方向，如果出现 `left | right` 或 `up | down` 这种情况会自动抵消。
         */
        constexpr combined_direction &operator|=(combined_direction &lhs,
                                                 combined_direction  rhs) noexcept {
            lhs = lhs | rhs;
            return lhs;
        }

        /**
         * @brief 综合方向是不是单一方向（仅有上下左右）。
         * @param [in] cdir 综合方向。
         * @return true 是单一方向。
         * @return false 不是单一方向。
         */
        constexpr bool is_orthogonal_directions(combined_direction cdir) noexcept {
            auto val = std::to_underlying(cdir);
            return (val != 0 && (val & (val - 1u)) == 0);
        }

        /**
         * @brief 综合方向是不是斜对角方向。
         * @param [in] cdir 综合方向。
         * @return true 是斜对角方向。
         * @return false 不是斜对角方向。
         */
        constexpr bool is_diagonal_directions(combined_direction cdir) noexcept {
            auto val = std::to_underlying(cdir);
            return (val & (val - 1u)) != 0;
        }

        /**
         * @brief 从单一方向转为综合方向。
         * @param [in] dir 单一方向。
         * @return combined_direction 转换后的综合方向。
         */
        constexpr combined_direction direction_to_combined_direction(direction dir) noexcept {
            THR_ASSERT_MSG(std::to_underlying(direction::right) <= std::to_underlying(dir)
                               && std::to_underlying(direction::up) >= std::to_underlying(dir),
                           "参数 dir（值为：{}）非法", std::to_underlying(dir));
            // 将 switch-case 表优化。
            return static_cast<combined_direction>(1u << std::to_underlying(dir));
        }

        /**
         * @brief 从综合方向转为单一方向。
         * @param [in] cdir 综合方向。
         * @return std::optional<direction> 转换后的单一方向。
         * @retval std::nullopt 参数不包含方向或包含两个方向，无法转换。
         */
        constexpr std::optional<direction>
        combined_direction_to_direction(combined_direction cdir) noexcept {
            if (is_orthogonal_directions(cdir)) {
                // 将 switch-case 表优化。
                return static_cast<direction>(std::countr_zero(std::to_underlying(cdir)));
            }
            return std::nullopt;
        }

        /**
         * @brief 从综合方向转为单一方向，若失败则返回默认值。
         * @param [in] cdir 综合方向。
         * @param [in] default_dir 默认值。
         * @return direction 转换后的单一方向。
         */
        constexpr direction combined_direction_to_direction_or(combined_direction cdir,
                                                               direction          default_dir) noexcept {
            auto opt = combined_direction_to_direction(cdir);
            return opt.value_or(default_dir);
        }

        /**
         * @brief 综合方向是否拥有单一方向。
         * @param [in] cdir 综合方向。
         * @param [in] dir 单一方向。
         * @return true 综合方向拥有单一方向。
         * @return false 综合方向不拥有单一方向。
         */
        constexpr bool has_direction(combined_direction cdir, direction dir) noexcept {
            return (std::to_underlying(cdir) & std::to_underlying(direction_to_combined_direction(dir)))
                   != 0;
        }

        /**
         * @brief 获取水平方向的分方向。
         * @param [in] cdir 综合方向。
         * @return combined_direction 水平方向的分方向。
         */
        constexpr combined_direction get_horizontal_component(combined_direction cdir) noexcept {
            return static_cast<combined_direction>(
                std::to_underlying(cdir)
                & static_cast<std::underlying_type_t<combined_direction>>(
                    std::to_underlying(combined_direction::left)
                    | std::to_underlying(combined_direction::right)));
        }

        /**
         * @brief 获取竖直方向的分方向。
         * @param [in] cdir 综合方向。
         * @return combined_direction 竖直方向的分方向。
         */
        constexpr combined_direction get_vertical_component(combined_direction cdir) noexcept {
            return static_cast<combined_direction>(
                std::to_underlying(cdir)
                & static_cast<std::underlying_type_t<combined_direction>>(
                    std::to_underlying(combined_direction::up)
                    | std::to_underlying(combined_direction::down)));
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