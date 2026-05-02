/**
 * @file scene_components.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 定义了总场景组件和获取父场景实体的组件
 * @version 0.1.0-1
 * @date 2026-05-02
 * 
 * @copyright cpp-love
 * 
 * @details
 * - 使用 `level_identifier_type` 来代指场景
 * - 使用 `std::set<entt::entity>` 来存储场景的子实体
 * - 使用 `std::set<level_identifier_type>` 来存储父场景id
 * 
 */

#ifndef THR_ECS_COMPONENTS_GLOBAL_SCENE_COMPONENTS_HPP
#define THR_ECS_COMPONENTS_GLOBAL_SCENE_COMPONENTS_HPP

#include "thr/base/config.hpp"
#include <entt/fwd.hpp>
#include <map>
#include <set>

namespace thr::ecs {

    /// @brief 总场景组件
    struct game_scenes final {
        std::map<level_identifier_type, std::set<entt::entity>>
            m_scenes_childrens; //< 映射：场景id -> 子实体列表
    };

    /// @brief 获取父场景实体的组件
    struct father_scenes final {
        std::set<level_identifier_type> m_fathers; //< 父场景id
    };

} // namespace thr::ecs

#endif // THR_ECS_COMPONENTS_GLOBAL_SCENE_COMPONENTS_HPP