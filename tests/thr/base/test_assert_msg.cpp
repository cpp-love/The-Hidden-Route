/**
 * @file test_assert_msg.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief `THR_ASSERT_MSG` 和 `thr::unreachable` 的测试用例和使用示例。
 * @version 0.1.0-1
 * @date 2026-05-02
 * 
 * @copyright cpp-love
 * 
 */

#undef NDEBUG
#include "thr/base/assert_msg.hpp"

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

int main() {

#ifdef _WIN32
    // 让Windows支持UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif // _WIN32

    int var = 10; //NOLINT(cppcoreguidelines-avoid-magic-numbers)

    // 1. 编译时字符串（与下面的 2. 原理相同）
    THR_ASSERT_MSG(var > 9, "var 必须大于9");

    // 2. 运行时字符串
    std::string message = "var 必须大于9，因为 var 为";
    message += std::format("{}", var);
    THR_ASSERT_MSG(var > 9, message);

    // 3. 没有字符串
    THR_ASSERT_MSG(var > 0);

    // 这个断言一定会被触发
    // 4. 编译时格式化字符串+格式化参数
    THR_ASSERT_MSG(var < 8, "这个断言一定会被触发，因为var为 {} ，大于8", var);

    // 下面不会运行到
    thr::unreachable("这句话不会被打印, {}", "^_^");
    thr::unreachable("这句话不会被打印");
    thr::unreachable(message);
    thr::unreachable();

    return 0;
}