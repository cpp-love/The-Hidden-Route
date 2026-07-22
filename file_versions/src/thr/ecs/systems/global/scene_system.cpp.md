<!--! \page scene_system_cpp scene_system.cpp 版本历史 -->

# scene_system.cpp 版本历史

## \[0.1.0-2] - 2026-07-22 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Fixed(bug 修复)

1. 修复了在 `registry.destroy(<entity>)` 时触发 `on_destroy` 回调后调用场景管理系统时访问当前实体的组件导致的潜在崩溃问题。

## \[0.1.0-1] - 2026-05-02 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 实现了场景管理系统(`thr::ecs::scene_system`)及其功能。
