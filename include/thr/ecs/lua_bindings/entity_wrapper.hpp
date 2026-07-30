/**
 * @file entity_wrapper.hpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 定义了实体包装器。
 * @version 0.1.0-3
 * @date 2026-07-29
 * 
 * @copyright cpp-love
 * 
 */

#ifndef THR_ECS_LUA_BINDINGS_ENTITY_WRAPPER_HPP
#define THR_ECS_LUA_BINDINGS_ENTITY_WRAPPER_HPP

#include "thr/ecs/components/global/game_base.hpp"
#include "thr/ecs/components/level_components.hpp"
#include "thr/ecs/lua_bindings/process_sequence.hpp"
#include <entt/entity/entity.hpp>
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>
#include <vector>

namespace thr::ecs::lua_bindings {

    /// @brief 给 Lua 的实体包装器。
    class entity_wrapper {
      private:
        entt::handle m_handle; ///< 当前实体句柄。
      public:
        /**
         * @brief 构造实体包装器。
         * @param [in] registry 实体所属的注册表。
         * @param [in] entity 实体。
         */
        entity_wrapper(entt::registry &registry, entt::entity entity) noexcept
            : m_handle(registry, entity) {}

        /**
         * @brief 构造实体包装器。
         * @param [in] handle 实体句柄。
         */
        explicit entity_wrapper(entt::handle handle) noexcept : m_handle(handle) {}

        /**
         * @brief 获取原始实体。
         * @return entt::entity 原始实体。
         * @warning 此函数不应暴露给 Lua！
         */
        [[nodiscard]] entt::entity    get_raw_entity() const noexcept { return m_handle.entity(); }

        /**
         * @brief 获取注册表。
         * @return entt::registry& 注册表。
         * @warning 此函数不应暴露给 Lua！
         */
        [[nodiscard]] entt::registry &get_registry() const noexcept { return *m_handle.registry(); }

        /**
         * @brief 制造实体的句柄。
         * @return entt::handle 实体的句柄。
         * @warning 此函数不应暴露给 Lua！
         */
        [[nodiscard]] entt::handle    get_handle() const noexcept { return m_handle; }

        /**
         * @brief 检测实体是否合法。
         * @return true 实体合法。
         * @return false 实体不合法。
         */
        [[nodiscard]] bool            valid() const noexcept { return m_handle.valid(); }

        /// @brief 销毁该实体。
        void                          destroy() { m_handle.destroy(); }

        /**
         * @brief 为实体添加标签。
         * @param [in] tag_id 标签编号。
         * @return true 添加成功。
         * @return false 添加失败：实体不合法或标签已经添加过。
         */
        bool                          add_tag(int tag_id) {
            if (!valid()) {
                return false;
            }
            auto &cur_tag = m_handle.get_or_emplace<tag>();
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
            auto &cur_tag = m_handle.get_or_emplace<tag>();
            return cur_tag.tag_ids.erase(tag_id) != 0u;
        }

        /**
         * @brief 查询实体是否拥有指定标签。
         * @param [in] tag_id 标签编号。
         * @return true 拥有此标签。
         * @return false 不拥有此标签。
         */
        [[nodiscard]] bool query_tag(int tag_id) const {
            if (!valid()) {
                return false;
            }
            auto *cur_tag = m_handle.try_get<tag>();
            if (cur_tag == nullptr) {
                return false;
            }
            return cur_tag->tag_ids.contains(tag_id);
        }

        /**
         * @brief 获取实体的所有标签。
         * @return std::vector<int> 所有标签。
         */
        [[nodiscard]] std::vector<int> get_tags() const {
            if (!valid()) {
                return {};
            }
            auto *cur_tag = m_handle.try_get<tag>();
            if (cur_tag == nullptr) {
                return {};
            }
            return cur_tag->tag_ids | std::ranges::to<std::vector>();
        }

        /**
         * @brief 创建实体的进程序列。
         * @return process_sequence 实体的进程序列。
         */
        [[nodiscard]] process_sequence create_process_sequence() {
            auto &ctx = get_registry().ctx();
            if (!ctx.contains<scheduler>()) {
                ctx.emplace<scheduler>();
            }
            return process_sequence{m_handle, ctx.get<scheduler>()};
        }
    };

    /**
     * @brief 比较两个实体包装器是否相等。
     * @param [in] lhs 左侧实体包装器。
     * @param [in] rhs 右侧实体包装器。
     * @return true 两个实体包装器相等。
     * @return false 两个实体包装器不相等。
     */
    inline bool operator==(const entity_wrapper &lhs, const entity_wrapper &rhs) noexcept {
        return lhs.get_handle() == rhs.get_handle();
    }

} // namespace thr::ecs::lua_bindings

#endif // THR_ECS_LUA_BINDINGS_ENTITY_WRAPPER_HPP