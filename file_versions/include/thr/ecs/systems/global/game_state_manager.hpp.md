<!--! \page game_state_manager_hpp game_state_manager.hpp 版本历史 -->

# game_state_manager.hpp 版本历史

## \[0.1.0-4] - 2026-07-14 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Changed(修改)

1. 为修复 bug 修改内部成员。

### Fixed(bug 修复)

1. 修复了通过事件触发添加关卡时添加的关卡在 `thr::ecs::game_state_base::init` 虚函数中向事件触发器（`entt::dispatcher`）中添加回调时会产生 UB 的问题。

## \[0.1.0-3] - 2026-07-12 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 在 `m_gui` 中默认添加一个有游戏界面大小的 UI 面板组件成员，
   - 其渲染在屏幕上的大小会与 SFML 的游戏渲染界面渲染在屏幕上的大小相同。
   - **注意：** 其逻辑大小与 SFML 的游戏渲染界面的逻辑大小不同！
2. 添加静态成员 `thr::ecs::game_state_manager::game_screen_panel_name`，用于提供上述的有游戏界面大小的 UI 面板组件的名称。
3. 在 `thr::ecs::game_state_manager::handle_event` 函数中自动处理 `sf::Event::Closed` 和 `sf::Event::Resized` 事件
   - 对于 `sf::Event::Closed`，采取直接关闭窗口。
   - 对于 `sf::Event::Resized`，采取自动调整 SFML 窗口的视图，并调整上述的有游戏界面大小的 UI 面板组件的大小和位置，使其与 SFML 窗口匹配。
4. 为 `m_gui` 添加默认字体。

## \[0.1.0-2] - 2026-06-19 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 为 `thr::ecs::game_state_manager` 添加数据成员 `m_window`, `m_gui`。

### Changed(修改)

1. **BREKING(不向下兼容):** 在 `thr::ecs::game_state_manager` 的构造函数中添加 `window` 参数。
2. **BREKING(不向下兼容):** 删除 `thr::ecs::game_state_manager::draw` 的 `render` 参数，改为使用数据成员 `m_window`。

## \[0.1.0-1] - 2026-05-02 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 添加了游戏状态管理系统(`thr::ecs::game_state_manager`)及其功能。
