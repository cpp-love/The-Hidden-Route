<!--! \page game_state_components_hpp game_state_components.hpp 版本历史 -->

# game_state_components.hpp 版本历史

## \[0.1.0-2] - 2026-06-19 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 为 `thr:ecs::game_state_base` 添加数据成员 `m_window` 和 `m_global_gui`。
2. 添加 `thr::ecs::game_state_base::init` 方法，用于二次构造。

### Changed(修改)

1. **BREKING(不向下兼容):** 删除 `thr::ecs::game_state_base::draw` 方法的 `render` 参数，请改用数据成员 `m_window` 绘制（`*m_window` 相当于 `render` 参数）。
2. **BREKING(不向下兼容):** 为 `thr::ecs::game_state_base::bind` 方法添加 `window` 和 `global_gui` 参数。

## \[0.1.0-1] - 2026-05-02 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 声明了游戏状态基类 `game_state_base`。
