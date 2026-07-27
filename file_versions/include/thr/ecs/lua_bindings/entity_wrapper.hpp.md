<!--! \page entity_wrapper_hpp entity_wrapper.hpp 版本历史 -->

# entity_wrapper.hpp 版本历史

## \[0.1.0-2] - 2026-07-27 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 添加 `get_handle` 成员函数和从 `entt::handle` 构造的构造函数。
2. 添加 `create_process_sequence` 成员函数，用于创建该实体的进程序列。

### Changed(修改)

1. 将其内部实现方式改为使用 `entt::handle`。
2. **BREKING(不向下兼容):** 将 `entt::registry` 相关函数从使用 `std::reference_wrapper` 包装改为使用引用包装。

## \[0.1.0-1] - 2026-07-08 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 定义了实体包装器 `thr::ecs::lua_bindings::entity_wrapper`。
