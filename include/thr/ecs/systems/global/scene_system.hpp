/**
 * @file scene_system.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 声明了游戏场景系统。
 * @version 0.1.0-2
 * @date 2026-06-07
 * 
 * @copyright cpp-love
 * 
 * @details
 * - 此游戏状态系统为内嵌式系统，即与 `entt::registry` 绑定。
 * 
 */

#ifndef THR_ECS_SYSTEMS_GLOBAL_SCENE_SYSTEM_HPP
#define THR_ECS_SYSTEMS_GLOBAL_SCENE_SYSTEM_HPP

#include "thr/base/config.hpp"
#include "thr/ecs/components/global/scene_components.hpp"
#include <entt/entity/sparse_set.hpp>
#include <entt/fwd.hpp>
#include <generator>

namespace thr::ecs {

    /// @brief 基本游戏场景系统
    class scene_system {
      public:
        /**
         * @brief 插入一个场景实体。
         * @param [in] registry 注册表。
         * @param [in] scene_id 场景id。
         * @return true 成功，场景之前未创建过。
         * @return false 失败，场景之前已创建过。
         */
        static bool insert_scene(entt::registry &registry, scene_identifier_type scene_id) noexcept;
        /**
         * @brief 删除一个场景实体。
         * @param [in] registry 注册表。
         * @param [in] scene_id 场景id。
         * @return true 删除成功，场景之前存在。
         * @return false 删除失败，场景之前不存在。
         * @warning 此函数不会删除场景的子实体，需要手动删除。
         */
        static bool erase_scene(entt::registry &registry, scene_identifier_type scene_id) noexcept;
        /**
         * @brief 清除所有的场景实体
         * @param [in] registry 注册表
         * @warning 此函数不会删除场景的子实体，需要手动删除
         */
        static void clear_scenes(entt::registry &registry) noexcept;
        /**
         * @brief 获取所有场景实体的列表。
         * @param [in] registry 注册表。
         * @return const std::map<scene_identifier_type, std::set<entt::entity>>& 所有的场景及其子实体。
         */
        [[nodiscard]] static const std::map<scene_identifier_type, std::set<entt::entity>>             &
        get_scenes(entt::registry &registry) noexcept;
        /**
         * @brief 将子实体加入场景中。
         * @param [in] registry 注册表。
         * @param [in] scene_id 场景id。
         * @param [in] child_entity 子实体。
         * @return true 成功，子实体之前未加入过。
         * @return false 失败，子实体之前已加入过。
         */
        static bool insert_to_scene(entt::registry &registry, scene_identifier_type scene_id,
                                    entt::entity child_entity) noexcept;
        /**
         * @brief 将子实体从场景中移除。
         * @param [in] registry 注册表。
         * @param [in] scene_id 场景id。
         * @param [in] child_entity 子实体。
         * @return true 成功，子实体之前已加入过。
         * @return false 失败，子实体之前未加入过。
         */
        static bool erase_from_scene(entt::registry &registry, scene_identifier_type scene_id,
                                     entt::entity child_entity) noexcept;
        /**
         * @brief 将子实体从所有场景中移除。
         * @param [in] registry 注册表。
         * @param [in] child_entity 子实体。
         */
        static void erase_from_all_scenes(entt::registry &registry, entt::entity child_entity) noexcept;
        /**
         * @brief 获取场景所有孩子的集合
         * @param [in] registry 注册表
         * @param [in] scene_id 场景id。
         * @return const std::set<entt::entity>& 场景所有孩子的集合。
         */
        [[nodiscard]] static const std::set<entt::entity> &
        get_scene_children(entt::registry &registry, scene_identifier_type scene_id) noexcept;
        /**
         * @brief 获取子实体所有父场景的集合。
         * @param [in] registry 注册表。
         * @param [in] child_entity 子实体。
         * @return const std::set<scene_identifier_type>& 子实体所有父场景的集合。
         */
        [[nodiscard]] static const std::set<scene_identifier_type> &
        get_father_scenes(entt::registry &registry, entt::entity child_entity) noexcept;

        /**
         * @brief 获取与指定实体在同一场景内的实体列表。
         * @param [in] registry 注册表。
         * @param [in] entity 指定的实体。
         * @return std::generator<const entt::entity &> 实体列表生成器，保证生成的列表中没有重复的元素且不包含传入实体。
         */
        [[nodiscard]] static std::generator<const entt::entity &>
        get_entities_in_same_scene(entt::registry &registry, entt::entity entity) noexcept {
            entt::sparse_set entity_set;
            entity_set.push(entity);
            const auto &father_scenes = get_father_scenes(registry, entity);
            for (scene_identifier_type scene_id : father_scenes) {
                for (entt::entity current_entity : get_scene_children(registry, scene_id)) {
                    if (!entity_set.contains(current_entity)) {
                        entity_set.push(current_entity);
                        co_yield current_entity;
                    }
                }
            }
        }
    };

} // namespace thr::ecs

#endif // THR_ECS_SYSTEMS_GLOBAL_SCENE_SYSTEM_HPP