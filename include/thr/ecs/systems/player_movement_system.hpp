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

#include "thr/base/with_history.hpp"
#include "thr/ecs/components/global/game_base.hpp"
#include <entt/entity/fwd.hpp>
#include <unordered_set>

namespace thr::ecs {

    /// @brief 版本更新控制器。
    class version_update_controller {
      private:
        std::unordered_set<entt::entity> m_updated_entity; ///< 已经更新的实体。

      public:
        /**
         * @brief 获取已经更新的实体。
         * @return const std::unordered_set<entt::entity>& 已经更新的实体。
         */
        [[nodiscard]] constexpr const std::unordered_set<entt::entity> &
        get_updated_entities() const noexcept {
            return m_updated_entity;
        }

        /// @brief 清空已经更新的实体。
        constexpr void clear() noexcept { m_updated_entity.clear(); }

        /**
         * @brief 保存当前版本，如果之前没有保存过。
         * @tparam T 有版本的类型。
         * @param [in] entity 实体。
         * @param [in, out] history 版本。
         * @return T& 可修改的最新状态，无论如何都与修改前的最新状态持有的数据一致。
         */
        template <typename T>
        constexpr T &store_version(entt::entity entity, with_history<T> &history) {
            if (m_updated_entity.emplace(entity).second) {
                return history.store_current_state(history.get_current_state());
            }
            return history.get_current_state_modifiable();
        }
    };

    /// @brief 玩家移动系统。
    class player_movement_system {
      public:
        static constexpr float move_epsilon = 3.f; //< 移动的容错间隔。

        /**
         * @brief 尝试移动玩家。
         * @param [in] registry 注册表。
         * @param [in] player_entity 玩家实体。
         * @param [in] delta_length 更新距离。
         * @param [in] cdir 玩家行走方向（综合方向）。
         * @param [in] controller 版本更新控制器。
         */
        static void try_move(entt::registry &registry, entt::entity player_entity, float delta_length,
                             combined_direction cdir, version_update_controller &controller);
    };
} // namespace thr::ecs

#endif // THR_ECS_SYSTEMS_PLAYER_MOVEMENT_SYSTEM_HPP