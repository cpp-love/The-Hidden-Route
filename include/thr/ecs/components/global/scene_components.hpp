/**
 * @file scene_components.hpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 定义了总场景组件和获取父场景实体的组件
 * @version 0.1.0-1
 * @date 2026-05-02
 * 
 * @copyright cpp-love
 * 
 * @details
 * - 使用 `scene_identifier_type` 来代指场景
 * - 使用 `std::set<entt::entity>` 来存储场景的子实体
 * - 使用 `std::set<scene_identifier_type>` 来存储父场景id
 * 
 */

#ifndef THR_ECS_COMPONENTS_GLOBAL_SCENE_COMPONENTS_HPP
#define THR_ECS_COMPONENTS_GLOBAL_SCENE_COMPONENTS_HPP

#include <entt/fwd.hpp>
#include <limits>
#include <map>
#include <set>

namespace thr::ecs {

    using scene_identifier_type = std::uint16_t; ///< 场景标识符类型。

    /**
     * @brief 使用行列制作场景标识符。
     * @param [in] row 行。
     * @param [in] col 列。
     * @return scene_identifier_type 场景标识符。
     */
    constexpr scene_identifier_type make_scene_identifier(std::uint8_t row, std::uint8_t col) {
        return static_cast<scene_identifier_type>(
                   row << static_cast<std::uint8_t>(std::numeric_limits<std::uint8_t>::digits))
               | static_cast<scene_identifier_type>(col);
    }

    /// @brief 总场景组件
    struct game_scenes final {
        std::map<scene_identifier_type, std::set<entt::entity>>
            m_scenes_childrens; //< 映射：场景id -> 子实体列表。
    };

    /// @brief 获取父场景实体的组件
    struct father_scenes final {
        std::set<scene_identifier_type> m_fathers; //< 父场景id。
    };

} // namespace thr::ecs

#endif // THR_ECS_COMPONENTS_GLOBAL_SCENE_COMPONENTS_HPP