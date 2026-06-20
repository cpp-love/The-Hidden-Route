\page file_versions sfml_helper.hpp 版本历史
\tableofcontent

# sfml_helper.hpp 版本历史

## \[0.1.0-2] - 2026-06-20 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. 添加了 `nlohmann::adl_serializer` 对于 `sf::Color`, `sf::Vector2` 和 `sf::Vector3` 的特化，用于序列化。

### Changed(修改)

1. **BREKING(不向下兼容):** 将文件名从 `sfml_formatter.hpp` 改为 `sfml_helper.hpp`。

### Fixed(bug 修复)

1. 修复了 `std::formatter` 特化中对 `ctx.out()` 的误用。

## \[0.1.0-1] - 2026-05-01 - cpp-love(<15865418+cpp-love@user.noreply.gitee.com>)

### Added(新增)

1. 添加了 `std::formatter` 对于 `sf::Vector2` 和 `sf::Vector3` 的特化，用于基本的格式化支持。

---
