/**
 * @file player_move_command.hpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 定义了玩家移动的操作。
 * @version 0.1.0-1
 * @date 2026-08-17
 * 
 * @copyright cpp-love
 * 
 */

#ifndef THR_UNDO_PLAYER_MOVE_COMMAND_HPP
#define THR_UNDO_PLAYER_MOVE_COMMAND_HPP

#include "thr/ecs/components/maze_components.hpp"
#include "thr/ecs/components/player_components.hpp"
#include "thr/ecs/systems/player_movement_system.hpp"
#include "thr/undo/command_base.hpp"
#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
#include <functional>

namespace thr::undo {

    /// @brief 玩家移动命令。
    class player_move_command : public command_base {
      public:
        static constexpr float velocity_per_millisecond = 0.2f; ///< 玩家移动速度。

      private:
        std::reference_wrapper<entt::registry> m_registry;                            ///< 注册表。
        entt::entity                           m_player_entity{entt::null};           ///< 玩家实体。
        thr::ecs::combined_direction m_direction{thr::ecs::combined_direction::none}; ///< 玩家移动方向。
        thr::ecs::version_update_controller m_controller; ///< 版本更新控制器。

      public:
        /**
         * @brief 构造玩家移动命令对象。
         * @param [in] registry 注册表。
         * @param [in] player_entity 玩家实体。
         * @param [in] dir 玩家移动方向。
         */
        constexpr player_move_command(std::reference_wrapper<entt::registry> registry,
                                      entt::entity                           player_entity,
                                      thr::ecs::combined_direction           dir) noexcept
            : m_registry(registry), m_player_entity(player_entity), m_direction(dir) {}

        /// @copydoc command_base::update
        void update(thr::ecs::milliseconds_f delta_time) override {
            ecs::player_movement_system::try_move(m_registry, m_player_entity,
                                                  delta_time.count() * velocity_per_millisecond,
                                                  m_direction, m_controller);
        }

        /// @copydoc command_base::undo
        void undo() override {
            for (const entt::entity entity : m_controller.get_updated_entities()) {
                if (entity == m_player_entity) {
                    auto &cur_player = m_registry.get().get<thr::ecs::player>(entity);
                    cur_player.statuses.switch_to_previous();
                    continue;
                }
                auto &seg = m_registry.get().get<thr::ecs::segment>(entity);
                seg.infos.switch_to_previous();
            }
        }

        /// @copydoc command_base::redo
        void redo() override {
            for (const entt::entity entity : m_controller.get_updated_entities()) {
                if (entity == m_player_entity) {
                    auto &cur_player = m_registry.get().get<thr::ecs::player>(entity);
                    cur_player.statuses.switch_to_next();
                    continue;
                }
                auto &seg = m_registry.get().get<thr::ecs::segment>(entity);
                seg.infos.switch_to_next();
            }
        }
    };

} // namespace thr::undo

#endif // THR_UNDO_PLAYER_MOVE_COMMAND_HPP