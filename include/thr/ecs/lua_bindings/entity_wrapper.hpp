/**
 * @file entity_wrapper.hpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 定义了实体包装器。
 * @version 0.1.0-1
 * @date 2026-07-08
 * 
 * @copyright cpp-love
 * 
 */

#ifndef THR_ECS_LUA_BINDINGS_ENTITY_WRAPPER_HPP
#define THR_ECS_LUA_BINDINGS_ENTITY_WRAPPER_HPP

#include "thr/ecs/components/level_components.hpp"
#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>
#include <functional>

namespace thr::ecs::lua_bindings {

    /// @brief 给 Lua 的实体包装器。
    class entity_wrapper {
      private:
        entt::entity                           m_entity;   ///< 当前实体。
        std::reference_wrapper<entt::registry> m_registry; ///< 当前实体所属的注册表。
      public:
        /**
         * @brief 构建一个实体包装器。
         * @param [in] entity 实体。
         * @param [in] registry 实体所属的注册表。
         */
        entity_wrapper(entt::entity entity, std::reference_wrapper<entt::registry> registry) noexcept
            : m_entity(entity), m_registry(registry) {}

        /**
         * @brief 检测实体是否合法。
         * @return true 实体合法。
         * @return false 实体不合法。
         */
        [[nodiscard]] bool         valid() const { return m_registry.get().valid(m_entity); }

        /**
         * @brief 获取原始实体。
         * @return entt::entity 原始实体。
         * @warning 此函数不应暴露给 Lua！
         */
        [[nodiscard]] entt::entity get_raw_entity() const noexcept { return m_entity; }

        /**
         * @brief 获取注册表。
         * @return entt::entity 注册表。
         * @warning 此函数不应暴露给 Lua！
         */
        [[nodiscard]] std::reference_wrapper<entt::registry> get_registry() const noexcept {
            return m_registry;
        }

        /// @brief 销毁该实体。
        void destroy() { m_registry.get().destroy(m_entity); }

        /**
         * @brief 为实体添加标签。
         * @param [in] tag_id 标签编号。
         * @return true 添加成功。
         * @return false 添加失败：实体不合法或标签已经添加过。
         */
        bool add_tag(int tag_id) {
            if (!valid()) {
                return false;
            }
            auto &cur_tag = m_registry.get().get_or_emplace<tag>(m_entity);
            return cur_tag.tag_ids.insert(tag_id).second;
        }

        /**
         * @brief 为实体删除标签。
         * @param [in] tag_id 标签编号。
         * @return true 删除成功。
         * @return false 删除失败：实体不合法或标签没有添加。
         */
        bool remove_tag(int tag_id) {
            if (!valid()) {
                return false;
            }
            auto &cur_tag = m_registry.get().get_or_emplace<tag>(m_entity);
            return cur_tag.tag_ids.erase(tag_id) != 0u;
        }

        /**
         * @brief 查询实体是否拥有标签。
         * @param [in] tag_id 标签编号。
         * @return true 拥有此标签。
         * @return false 不拥有此标签。
         */
        [[nodiscard]] bool query_tag(int tag_id) const {
            if (!valid()) {
                return false;
            }
            const auto *cur_tag = m_registry.get().try_get<tag>(m_entity);
            if (cur_tag == nullptr) {
                return false;
            }
            return cur_tag->tag_ids.contains(tag_id);
        }
    };

    /**
     * @brief 比较两个实体包装器是否相等。
     * @param [in] lhs 左侧实体包装器。
     * @param [in] rhs 右侧实体包装器。
     * @return true 两个实体包装器相等。
     * @return false 两个实体包装器不相等。
     */
    inline bool operator==(const entity_wrapper &lhs, const entity_wrapper &rhs) {
        return lhs.get_raw_entity() == rhs.get_raw_entity()
               && (&lhs.get_registry().get() == &rhs.get_registry().get());
    }

} // namespace thr::ecs::lua_bindings

#endif // THR_ECS_LUA_BINDINGS_ENTITY_WRAPPER_HPP