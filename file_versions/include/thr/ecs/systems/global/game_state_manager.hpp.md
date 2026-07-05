<!--! \page game_state_manager_hpp game_state_manager.hpp 版本历史 -->

# game_state_manager.hpp 版本历史

## \[0.1.0-2] - 2026-06-19 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 为 `thr::ecs::game_state_manager` 添加数据成员 `m_window`, `m_gui`。

### Changed(修改)

1. **BREKING(不向下兼容):** 在 `thr::ecs::game_state_manager` 的构造函数中添加 `window` 参数。
2. **BREKING(不向下兼容):** 删除 `thr::ecs::game_state_manager::draw` 的 `render` 参数，改为使用数据成员 `m_window`。

## \[0.1.0-1] - 2026-05-02 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 添加了游戏状态管理系统(`thr::ecs::game_state_manager`)及其功能。
