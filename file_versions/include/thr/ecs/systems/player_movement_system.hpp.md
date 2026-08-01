<!--! \page player_movement_system_hpp player_movement_system.hpp 版本历史 -->

# player_movement_system.hpp 版本历史

## \[0.1.0-3] - 2026-08-01 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Changed(修改)

1. **BREKING(不向下兼容):** 将 `thr::ecs::player_movement_system::update` 的返回值从 `void` 改为 `bool`。
2. 将 `thr::ecs::player_movement_system::move_epsilon` 从 5 改为 3。

## \[0.1.0-2] - 2026-07-22 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 添加函数 `thr::ecs::player_movement_system::update` 对综合方向的重载。

## \[0.1.0-1] - 2026-05-30 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 定义了玩家移动系统 `thr::ecs::player_movement_system`。
