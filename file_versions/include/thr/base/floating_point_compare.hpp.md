<!--! \page floating_point_compare_hpp floating_point_compare.hpp 版本历史 -->

# floating_point_compare.hpp 版本历史

## \[0.1.0-2] - 2026-07-21 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Changed(修改)

1. **BREKING(不向下兼容):** 为 `thr::no_nan_inf` 和 `thr::has_nan_inf` 的静态成员 `epsilon` 改为非静态的，允许用户自定义误差范围，在比较时取两个操作数定义的误差范围的较小值。

## \[0.1.0-1] - 2026-05-01 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 添加了 `thr::no_nan_inf` 和 `thr::has_nan_inf` 两个类模板，用于浮点数的近似比较。
