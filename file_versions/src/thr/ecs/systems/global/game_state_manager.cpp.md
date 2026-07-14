<!--! \page game_state_manager_cpp game_state_manager.cpp 版本历史 -->

# game_state_manager.cpp 版本历史

## \[0.1.0-6] - 2026-07-14 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Fixed(bug 修复)

1. 修复了 TGUI 会吞按键，导致 Crtl+Z 无法撤销，Escape 无法退出的问题。

## \[0.1.0-5] - 2026-07-14 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Fixed(bug 修复)

1. 修复了通过事件触发添加关卡时添加的关卡在 `thr::ecs::game_state_base::init` 虚函数中向事件触发器（`entt::dispatcher`）中添加回调时会产生 UB 的问题。

## \[0.1.0-4] - 2026-07-12 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Fixed(bug 修复)

1. 修复了按钮与点击位置错位的问题。

## \[0.1.0-3] - 2026-07-12 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 同步对 [game_state_manager.hpp](../../../../../../include/thr/ecs/systems/global/game_state_manager.hpp) 的新增。

## \[0.1.0-2] - 2026-06-19 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Changed(修改)

1. **BREKING(不向下兼容):** 同步 `thr::ecs::game_state_manager` 和 `thr::ecs::game_state_base` 的修改。

## \[0.1.0-1] - 2026-05-02 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 实现了游戏状态管理系统(`thr::ecs::game_state_manager`)及其功能。
