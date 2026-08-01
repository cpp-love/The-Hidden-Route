<!--! \page game_base_hpp game_base.hpp 版本历史 -->

# game_base.hpp 版本历史

## \[0.1.0-5] - 2026-08-01 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 添加函数 `thr::ecs::combined_direction_to_vector2f` 和 `thr::ecs::combined_direction_to_vector2f_2` 和 `thr::ecs::combined_direction` 的 `&` 和 `&=` 运算符重载。

### Changed(修改)

1. **BREKING(不向下兼容):** 将函数 `thr::ecs::is_orthogonal_directions` 和 `thr::ecs::is_diagonal_directions` 改名为 `thr::ecs::is_orthogonal` 和 `thr::ecs::is_diagonal`。

## \[0.1.0-4] - 2026-07-27 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 定义了自己的 `scheduler` 和 `process` 别名。
2. 添加字面量 `_ms_f` 对于 `long long` 类型的支持，让使用更方便。

## \[0.1.0-3] - 2026-07-22 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 为 `thr::ecs::direction` 添加相关函数 `is_horizontal` 和 `is_vertical`，用于判断方向是否水平/竖直。
2. 添加综合方向类 `thr::ecs::combined_direction` 和许多相关函数，用于更好地处理多种方向。

## \[0.1.0-2] - 2026-05-23 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 添加 `thr::ecs::block_side_length`, `thr::ecs::direction` 及其一些相关函数。（从文件 [`include/thr/ecs/components/maze_components.hpp`](../../../../../../include/thr/ecs/components/maze_components.hpp) 中移动而来）

## \[0.1.0-1] - 2026-05-02 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 定义了游戏的基础组件 `thr::ecs::clock`, `thr::ecs::milliseconds_f` 和 自定义字面量 `_ms_f`
