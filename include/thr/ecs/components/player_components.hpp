/**
 * @file player_components.hpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 定义了玩家相关的组件。
 * @version 0.1.0-1
 * @date 2026-05-23
 * 
 * @copyright cpp-love
 * 
 */

#ifndef THR_ECS_COMPONENTS_PLAYER_COMPONENTS_HPP
#define THR_ECS_COMPONENTS_PLAYER_COMPONENTS_HPP

#include "thr/ecs/components/global/game_base.hpp"
#include "thr/ecs/configs.hpp"
#include "thr/ecs/systems/global/scene_system.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>
#include <variant>
#include <vector>

namespace thr::ecs {

    /// @brief 在地面上的玩家。
    struct player_on_ground {
        /**
         * @brief 从配置中获取玩家颜色。
         * @return float 玩家颜色。
         */
        static sf::Color color() { return configs::singleton().player_on_ground_color; }
        /**
         * @brief 从配置中获取玩家边长。
         * @return float 玩家边长。
         */
        static float     side_length() { return configs::singleton().player_on_ground_side_length; }

        entt::entity     segment_entity; ///< 玩家所在线段所属的实体。
    };

    /// @brief 在地面下的玩家。
    struct player_under_ground {
        /**
         * @brief 从配置中获取玩家颜色。
         * @return sf::Color 玩家颜色。
         */
        static sf::Color color() { return configs::singleton().player_under_ground_color; }
        /**
         * @brief 从配置中获取玩家边长。
         * @return float 玩家边长。
         */
        static float     side_length() { return configs::singleton().player_under_ground_side_length; }

        sf::Vector2f     position;         ///< 玩家所在位置。
        std::optional<direction> prev_dir; ///< 玩家上一次走的方向。

        /**
         * @brief 在构造 @ref player_under_ground 时调用的函数，用于设置其所属场景。
         * @param [in] registry 注册表。
         * @param [in] entity 构造了 @ref player_under_ground 的实体。
         */
        static void              on_construct(entt::registry &registry, entt::entity entity) {
            const auto  &under_ground = registry.get<player_under_ground>(entity);
            sf::Vector2f start = under_ground.position
                                 - sf::Vector2f{player_under_ground::side_length() / 2,
                                                player_under_ground::side_length() / 2};
            sf::Vector2f end = under_ground.position
                               + sf::Vector2f{player_under_ground::side_length() / 2,
                                              player_under_ground::side_length() / 2};
            int          col_start = static_cast<int>(start.x / block_side_length);
            int          col_end = static_cast<int>(end.x / block_side_length);
            int          row_start = static_cast<int>(start.y / block_side_length);
            int          row_end = static_cast<int>(end.y / block_side_length);
            for (int row = row_start; row <= row_end; ++row) {
                for (int col = col_start; col <= col_end; ++col) {
                    auto scene_id = make_scene_identifier(row, col);
                    scene_system::insert_to_scene(registry, scene_id, entity);
                }
            }
        }

        /**
         * @brief 在更新 @ref player_under_ground 时调用的函数，用于设置其所属场景。
         * @param [in] registry 注册表。
         * @param [in] entity 更新了 @ref player_under_ground 的实体。
         */
        static void on_update(entt::registry &registry, entt::entity entity) {
            // 移除之前的场景登记，然后重新按当前矩形覆盖块插入
            scene_system::erase_from_all_scenes(registry, entity);
            on_construct(registry, entity);
        }

        /**
         * @brief 在移除 @ref player_under_ground 时调用的函数，用于设置其所属场景。
         * @param [in] registry 注册表。
         * @param [in] entity 移除了 @ref player_under_ground 的实体。
         */
        static void on_destroy(entt::registry &registry, entt::entity entity) {
            scene_system::erase_from_all_scenes(registry, entity);
        }

        /**
         * @brief 添加监听器，用于自动调用 @ref on_construct, @ref on_update, @ref on_destroy 来设置其所属场景。
         * @param [in] registry 注册表。
         */
        static void connect_listener(entt::registry &registry) {
            registry.on_construct<player_under_ground>().connect<&on_construct>();
            registry.on_update<player_under_ground>().connect<&on_update>();
            registry.on_destroy<player_under_ground>().connect<&on_destroy>();
        }

        /**
         * @brief 移除监听器，用于取消自动调用 @ref on_construct, @ref on_update, @ref on_destroy 来设置其所属场景。
         * @param [in] registry 注册表。
         */
        static void disconnect_listener(entt::registry &registry) {
            registry.on_construct<player_under_ground>().disconnect<&on_construct>();
            registry.on_update<player_under_ground>().disconnect<&on_update>();
            registry.on_destroy<player_under_ground>().disconnect<&on_destroy>();
        }
    };

    /// @brief 转向的历史记录。
    struct turning_history {
        std::vector<std::variant<player_on_ground, player_under_ground>> turnings;
    };

    /// @brief 关卡的基本信息。
    struct level_info {
        entt::entity start_segment_entity; ///< 起始段落对应的实体。
        entt::entity end_segment_entity;   ///< 结束段落对应的实体。
    };

} // namespace thr::ecs

#endif // THR_ECS_COMPONENTS_PLAYER_COMPONENTS_HPP