\page file_versions maze_components.hpp 版本历史
\tableofcontent

# maze_components.hpp 版本历史

## \[0.1.0-3] - 2026-06-19 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. 为类 `thr::ecs::segment` 添加方法 `get_bounds`, `get_walked_bounds`, `wrap_walked_precent`。
2. 为类 `thr::ecs::node` 添加静态常量数据成员 `side_length`。
3. 为类 `thr::ecs::line_strips` 添加静态常量数据成员 `default_width`。

### Changed(修改)

1. **BREAKING(不向下兼容):** 将文件名称从 `maze_line_components.hpp` 改为 `maze_components.hpp`。
2. **BREAKING(不向下兼容):** 将 `thr::ecs::block_side_length`, `thr::ecs::direction` 及其一些相关函数移动到文件 [`include/thr/ecs/components/global/game_base.hpp`](../../../../../include/thr/ecs/components/global/game_base.hpp) 中。
3. **BREAKING(不向下兼容):** 为避免实体所属场景处理错误，解除拥有 `thr::ecs::node` 的实体必须有 `thr::ecs::segment` 的限制，并为 `thr::ecs::node` 添加数据成员 `segment_entity` 表示指向的 `thr::ecs::segment` 所属的实体。
4. **BREKING(不向下兼容):** 修改了一些参数。

### Fixed(bug 修复)

1. 修复了 `thr::ecs::line_strips::draw` 绘制的折线不连续的问题。

## \[0.1.0-2] - 2026-05-04 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Changed(修改)

1. **BREAKING(不向下兼容):** 修改组件 `thr::ecs::segment` 和 `thr::ecs::node`。

## \[0.1.0-1] - 2026-05-02 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. 定义了迷宫线条组件 `thr::ecs::segment`, `thr::ecs::node`, `thr::ecs::line_strips`。
