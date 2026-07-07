<!--! \page assert_msg_hpp assert_msg.hpp 版本历史 -->

# assert_msg.hpp 版本历史

## \[0.1.0-1] - 2026-05-01 - cpp-love(<207296385+cpp-love@users.noreply.github.com>)

### Added(新增)

1. 添加了带消息的断言 `THR_ASSERT_MSG`，与 `<cassert>` 中的 `assert` 功能相近。 \
   发布时（定义了 `NDEBUG` 宏时）可以完全消除。
2. 添加了带消息的不可到达标记 `thr::unreachable`。 \
   编译时和发布时（定义了 `NDEBUG` 宏时）退化为 `std::unreachable`，调试时支持消息报错。
