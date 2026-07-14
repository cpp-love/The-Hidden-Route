<!--! \page level_render_system_cpp level_render_system.cpp 版本历史 -->

# level_render_system.cpp 版本历史

## \[0.1.0-4] - 2026-07-14 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Changed(修改)

1. **BREKING(不向下兼容):** 将文字和精灵调整至在最后渲染。

## \[0.1.0-3] - 2026-07-12 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 添加了对文字和精灵的渲染支持。

## \[0.1.0-2] - 2026-06-19 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 添加了可选的玩家的渲染。

### Fixed(bug 修复)

1. 修复了走过部分渲染会覆盖其他绘制元素（如 `thr::ecs::line_strips`）的问题。

## \[0.1.0-1] - 2026-05-02 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 定义了迷宫渲染系统 `thr::ecs::level_render_system`。
