<!--! \page game_states_hpp game_states.hpp 版本历史 -->

# game_states.hpp 版本历史

## \[0.1.0-5] - 2026-07-12 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 配合 [main/game_states.cpp](../../main/game_states.cpp) 添加对 Lua 脚本的支持，为 `mainhelper::game_screen` 添加数据成员 `m_lua_manager`。

## \[0.1.0-4] - 2026-07-07 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 添加关卡图界面 `mainhelper::level_graph_screen`。

## \[0.1.0-3] - 2026-07-01 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 配合 [main/game_states.cpp](../../main/game_states.cpp) 的修改，为 `mainhelper::game_screen` 添加数据成员 `m_winned_time`。

## \[0.1.0-2] - 2026-06-19 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Changed(修改)

1. **BREKING(不向下兼容):** 修改部分内容，使其配合 [game_states.cpp](../../main/game_states.cpp)。

## \[0.1.0-1] - 2026-05-02 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 声明了 4 个具体的游戏状态。
