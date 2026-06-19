/**
 * @file player_movement_system.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 声明了玩家移动系统。
 * @version 0.1.0-1
 * @date 2026-05-30
 * 
 * @copyright cpp-love
 * 
 */

#ifndef THR_ECS_SYSTEMS_PLAYER_MOVEMENT_SYSTEM_HPP
#define THR_ECS_SYSTEMS_PLAYER_MOVEMENT_SYSTEM_HPP

#include "thr/ecs/components/global/game_base.hpp"
#include "thr/ecs/components/player_components.hpp"
#include <entt/entity/fwd.hpp>

namespace thr::ecs {

    /// @brief 玩家移动系统。
    class player_movement_system {
      public:
        /**
         * @brief 更新玩家。
         * @param [in] registry 注册表。
         * @param [in] player_entity 玩家实体。
         * @param [in] delta_length 更新距离。
         * @param [in] dir 玩家行走方向。
         */
        static void update(entt::registry &registry, entt::entity player_entity, float delta_length,
                           direction dir) noexcept;
        /**
         * @brief 撤销上一次操作。
         * @param [in] registry 注册表。
         * @param [in] player_entity 玩家实体。
         */
        static void undo(entt::registry &registry, entt::entity player_entity) noexcept;
    };
} // namespace thr::ecs

#endif // THR_ECS_SYSTEMS_PLAYER_MOVEMENT_SYSTEM_HPP