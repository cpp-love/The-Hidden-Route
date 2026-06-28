\page file_versions level_render_system.cpp 版本历史
\tableofcontent

# level_render_system.cpp 版本历史

## \[0.1.0-2] - 2026-06-19 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. 添加了可选的玩家的渲染。

### Fixed(bug 修复)

1. 修复了走过部分渲染会覆盖其他绘制元素（如 `thr::ecs::line_strips`）的问题。

## \[0.1.0-1] - 2026-05-02 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. 定义了迷宫渲染系统 `thr::ecs::level_render_system`。
