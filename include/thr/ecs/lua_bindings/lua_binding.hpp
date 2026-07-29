/**
 * @file lua_binding.hpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 声明了绑定 C++ 相关内容到 Lua 脚本的函数。
 * @version 0.1.0-1
 * @date 2026-07-29
 * 
 * @copyright cpp-love
 * 
 */

#ifndef THR_ECS_LUA_BINDINGS_LUA_BINDING_HPP
#define THR_ECS_LUA_BINDINGS_LUA_BINDING_HPP

#include <entt/entity/fwd.hpp>
#include <sol/forward.hpp>

namespace thr::ecs::lua_bindings {

    /**
     * @brief 绑定 C++ 相关内容到 Lua 脚本。
     * @param [in] lua Lua 状态。
     * @param [in] registry 注册表。
     */
    void bind_to_lua(sol::state &lua, entt::registry &registry);

} // namespace thr::ecs::lua_bindings

#endif // THR_ECS_LUA_BINDINGS_LUA_BINDING_HPP