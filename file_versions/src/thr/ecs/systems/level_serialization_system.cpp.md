<!--! \page level_serialization_system_cpp level_serialization_system.cpp 版本历史 -->

# level_serialization_system.cpp 版本历史

## \[0.1.0-4] - 2026-07-12 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 同步 [`level.schema.json`](../../../../../schema/level-v0.1.0-2.schema.json) 的新增内容。

### Changed(修改)

1. 重构部分代码，提取重复部分。

## \[0.1.0-3] - 2026-07-02 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 用 `thr/base/sfml_helper.hpp` 完善了对 `sf::Color` 的序列化支持。

### Changed(修改)

1. **BREKING(不向下兼容):** 取消抛出 `std::invalid_argumens`，改为使用 `thr/base/sfml_helper.hpp` 中的异常。
2. 将 `sf::Vector2f` 的序列化改为使用 `thr/base/sfml_helper.hpp` 中的。

## \[0.1.0-2] - 2026-06-19 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Changed(修改)

1. **BREKING(不向下兼容):** 同步 [`level.schema.json`](../../../../../schema/level-v0.1.0-2.schema.json) 的修改。

### Fixed(bug 修复)

1. 修复了 `thr::ecs::level_serialization_system::deserialize_from_json` 中如果 json 文件没有 `line_strips` 一项则不会插入空的 `thr::ecs::line_strips` 的问题。

## \[0.1.0-1] - 2026-05-04 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 实现了迷宫序列化系统 `thr::ecs::level_serialization_system`。
