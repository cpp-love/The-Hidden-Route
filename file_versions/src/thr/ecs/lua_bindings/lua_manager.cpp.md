<!--! \page lua_manager_cpp lua_manager.cpp 版本历史 -->

# lua_manager.cpp 版本历史

## \[0.1.0-3] - 2026-07-27 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 添加 `thr::ecs::lua_bindings::progress_sequence` 类的绑定和 `thr::ecs::lua_bindings::entity_wrapper::create_progress_sequence` 函数的绑定。

## \[0.1.0-2] - 2026-07-22 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 在 Lua 发来的日志消息前添加 `[from Lua]` 标签，便于区分。

## \[0.1.0-1] - 2026-07-09 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 实现了与 Lua 沟通的管理类 `thr::ecs::lua_bindings::lua_manager`。
