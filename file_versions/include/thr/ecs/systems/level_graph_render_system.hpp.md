<!--! \page level_graph_render_system_hpp level_graph_render_system.hpp 版本历史 -->

# level_graph_render_system.hpp 版本历史

## \[0.1.0-2] - 2026-07-12 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Changed(修改)

1. **BREKING(不向下兼容):** 将 `thr::ecs::level_graph_render_system::draw` 中的参数 `gui` 修改为两个参数：
   1. 类型为 `tgui::Container::Ptr` 的参数 `container`，用于装 UI。
   2. 类型为 `sf::RenderTarget &` 的参数 `target`，用于绘制。
   - 迁移指南（从原来的 `gui` 参数转换）： `gui->getContainer()` 对应 `container`，`gui->getTarget()` 对应 `target`。

## \[0.1.0-1] - 2026-07-06 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 声明了渲染关卡图的系统 `thr::ecs::level_graph_render_system`。
