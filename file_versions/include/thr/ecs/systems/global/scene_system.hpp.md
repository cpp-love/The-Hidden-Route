\page file_versions scene_system.hpp 版本历史
\tableofcontent

# scene_system.hpp 版本历史

## \[0.1.0-2] - 2026-06-07 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Fixed(bug 修复)

1. 修复了 `thr::ecs::scene_system::get_entities_in_same_scene` 方法返回的列表生成器中会生成重复的元素和包含传入实体的问题。

## \[0.1.0-1] - 2026-05-02 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. 添加了场景管理系统(`thr::ecs::scene_system`)及其基本功能。
