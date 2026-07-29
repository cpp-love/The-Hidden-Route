<!--! \page lua_manager_hpp lua_manager.hpp 版本历史 -->

# lua_manager.hpp 版本历史

## \[0.1.0-2] - 2026-07-29 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Removed(移除)

1. 移除**整个文件**，改用原始的 `sol::state` + `thr/ecs/lua_bindings/lua_binding.hpp` 的注册函数。

## \[0.1.0-1] - 2026-07-08 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 定义了与 Lua 沟通的管理类 `thr::ecs::lua_bindings::lua_manager`。
