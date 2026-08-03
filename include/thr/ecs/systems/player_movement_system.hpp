/**
 * @file player_movement_system.hpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 定义了玩家移动系统。
 * @version 0.1.0-3
 * @date 2026-08-01
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
        static constexpr float move_epsilon = 3.f; //< 移动的容错间隔。
        /**
         * @brief 更新玩家。
         * @param [in] registry 注册表。
         * @param [in] player_entity 玩家实体。
         * @param [in] delta_length 更新距离。
         * @param [in] dir 玩家行走方向。
         * @return true 玩家移动了。
         * @return false 玩家没有移动。
         */
        static bool update(entt::registry &registry, entt::entity player_entity, float delta_length,
                           direction dir);
        /**
         * @brief 更新玩家。
         * @param [in] registry 注册表。
         * @param [in] player_entity 玩家实体。
         * @param [in] delta_length 更新距离。
         * @param [in] cdir 玩家行走方向（综合方向）。
         * @return true 玩家移动了。
         * @return false 玩家没有移动。
         */
        static bool update(entt::registry &registry, entt::entity player_entity, float delta_length,
                           combined_direction cdir);

        /**
         * @brief 撤销上一次操作。
         * @param [in] registry 注册表。
         */
        static void undo(entt::registry &registry);
    };
} // namespace thr::ecs

#endif // THR_ECS_SYSTEMS_PLAYER_MOVEMENT_SYSTEM_HPP