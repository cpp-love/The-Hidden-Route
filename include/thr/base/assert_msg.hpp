/**
 * @file assert_msg.hpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 添加了支持自定义消息的 `assert` 和支持自定义消息的 `std::unreachable`
 * @version 0.1.0-1
 * @date 2026-05-01
 * 
 * @copyright cpp-love
 * 
 */

#ifndef THR_BASE_ASSERT_MSG_HPP
#define THR_BASE_ASSERT_MSG_HPP

#include <concepts>
#include <exception>
#include <format>
#include <iterator>
#include <optional>
#include <ranges>
#include <source_location>
#include <spdlog/spdlog.h>
#include <stacktrace>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

/// @cond INTERNAL
/// @brief 一些内部实现，用户不应访问。
namespace thr::details {

    /**
     * @brief 获取堆栈信息。（默认为当前的）
     * @param [in] stack_trace 堆栈对象，默认为当前堆栈。
     * @return std::string 堆栈信息。
     */
    [[nodiscard]] inline std::string
    get_stack_trace_message(const std::stacktrace &stack_trace = std::stacktrace::current()) {
        std::string str;
        for (const auto &[index, entry] : stack_trace | std::views::enumerate) {
            std::format_to(std::back_inserter(str), "frame #{}: {}:{} in function: {}\n", index,
                           entry.source_file(), entry.source_line(), entry.description());
        }
        return str;
    }

    /**
     * @brief 断言失败处理函数。
     * @param [in] expr 断言表达式。
     * @param [in] loc 断言位置。
     * @param [in] message 断言失败时输出的消息。（默认为 `std::nullopt`)
     * @param [in] stack_trace 堆栈信息。（默认为当前堆栈）
     */
    [[noreturn]] inline void
    assert_fail(std::string_view expr, const std::source_location &loc,
                std::optional<std::string> message = std::nullopt,
                const std::stacktrace     &stack_trace = std::stacktrace::current()) noexcept {

        try {
            // 尝试完整记录日志。

            // 获取堆栈信息。
            std::string stack_trace_message;
            try {
                stack_trace_message = get_stack_trace_message(stack_trace);
            } catch (...) {
                stack_trace_message = "Due to an exception, we lost stack trace message.";
            }

            // 输出信息。
            if (message.has_value()) {
                spdlog::critical("Assertion failed at {}:{}:{} (in function :{}):\n"
                                 ">> Expression: {}\n"
                                 ">> Message: {}\n"
                                 "Stack trace:\n"
                                 "{}",
                                 loc.file_name(), loc.line(), loc.column(), loc.function_name(), expr,
                                 *message, stack_trace_message);
            } else {
                spdlog::critical("Assertion failed at {}:{}:{} (in function :{}):\n"
                                 ">> Expression: {}\n"
                                 "Stack trace:\n"
                                 "{}",
                                 loc.file_name(), loc.line(), loc.column(), loc.function_name(), expr,
                                 stack_trace_message);
            }

            // 及时刷新。
            spdlog::default_logger()->flush();
        } catch (...) {
            // 尝试简单日志。
            try {
                spdlog::critical("Assertion failed, and error logging threw an exception.\n");
                spdlog::default_logger()->flush();
            } catch (...) { // NOLINT(bugprone-empty-catch)
                // 静默退出。
            }
        }

        // 终止程序。
        std::terminate();
    }

    /**
     * @brief 断言检查函数。
     * @tparam Args 格式化字符串参数类型。
     * @param [in] condition 条件。
     * @param [in] expr 断言表达式。
     * @param [in] loc 断言位置。
     * @param [in] fmt 格式化字符串。
     * @param [in] args 格式化字符串参数。
     */
    template <typename... Args>
    constexpr void assert_check(bool condition, std::string_view expr, const std::source_location &loc,
                                std::format_string<Args...> fmt, Args &&...args) noexcept {
        if (condition) {
            return;
        }

        if (std::is_constant_evaluated()) {
            std::unreachable();
        } else {
            try {
                // 尝试完整信息。
                assert_fail(expr, loc, std::format(fmt, std::forward<Args>(args)...));
            } catch (...) {
                try {
                    // 尝试简短信息。
                    assert_fail(expr, loc, "Due to an exception, we lost assertion message.\n");
                } catch (...) {
                    // 静默报错。
                    assert_fail(expr, loc);
                }
            }
        }
    }

    /**
     * @brief 断言检查函数。
     * @param [in] condition 条件。
     * @param [in] expr 断言表达式。
     * @param [in] loc 断言位置。
     * @param [in] message 运行时消息。（如果有）
     */
    constexpr void assert_check(bool condition, std::string_view expr, const std::source_location &loc,
                                std::optional<std::string> message = std::nullopt) noexcept {
        if (condition) {
            return;
        }

        if (std::is_constant_evaluated()) {
            std::unreachable();
        } else {
            assert_fail(expr, loc, std::move(message));
        }
    }

    /**
     * @brief 判断类型是否为 `std::basic_format_string` 的实例化的 trait。
     * @tparam T 要判断的类型。
     */
    template <typename T>
    struct is_basic_format_string : std::false_type {};

    template <typename... Args>
    struct is_basic_format_string<std::basic_format_string<Args...>> : std::true_type {};

    /**
     * @brief 在构造原始类型的同时自动记录 `std::source_location` 的结构体。
     * @tparam T 原始类型。
     */
    template <typename T>
    struct with_source_location {
        T                    value; ///< 原始类型的值。
        std::source_location loc;   ///< 位置。

        // NOLINTBEGIN(hicpp-explicit-conversions)
        /**
         * @brief 构造一个包含 `T` 类型的对象。
         * @tparam U 传入的值的类型。
         * @param [in] val 传入的值。
         * @param [in] location 源码位置（可忽略，默认为调用的位置）。
         */
        template <typename U>
            requires std::constructible_from<T, U> && (!is_basic_format_string<T>::value)
        constexpr with_source_location(
            U &&val, std::source_location location =
                         std::source_location::current()) noexcept(std::is_nothrow_constructible_v<T, U>)
            : value(std::forward<U>(val)), loc(location) {}

        /**
         * @brief 构造一个包含 `T` 类型的对象。（对 `T` 是 `std::basic_format_string` 模板类的实例化这种情况的特化）
         * @tparam U 传入的值的类型。
         * @param [in] val 传入的值。
         * @param [in] location 源码位置（可忽略，默认为调用的位置）。
         * @details 为解决 MSVC 的编译问题，添加此特化，将原本的 `constexpr` 限定改为 `consteval`。
         */
        template <typename U>
            requires std::constructible_from<T, U> && is_basic_format_string<T>::value
        consteval with_source_location(
            U &&val, std::source_location location =
                         std::source_location::current()) noexcept(std::is_nothrow_constructible_v<T, U>)
            : value(std::forward<U>(val)), loc(location) {}
        // NOLINTEND(hicpp-explicit-conversions)
    };

} // namespace thr::details
/// @endcond

namespace thr {

    /**
     * @brief 带消息的 `std::unreachable`。（执行到正常情况无法到达的位置时的处理函数）
     * @param [in] message 若执行到不该执行的的此处时输出的消息。（默认为空，即 `std::nullopt`)
     * @details
     * - 当在编译期求值时，宏调用 `std::unreachable()`，**不输出消息**；
     *   当在运行期求值时：
     *   当定义宏 `NDEBUG` 时，函数只调用 `std::unreachable()`，**不输出消息**，若到达**会导致UB**；
     *   未定义宏 `NDEBUG` 时，会输出消息，行为定义。（调用 `std::terminate()` 终止函数）
     * - 输出格式：
     * ```plain
     * Control reached an unreachable place at <file>:<line>:<column> (in function :<function>):
     * >> Message: <message>
     * Stack trace:
     * frame #0 : <description> in line <line> in file <file>
     * ...
     * ```
     * @warning 此函数会调用 `std::terminate()` / `std::unreachable()`，不会返回，请确认调用处是否无法到达。
     */
    [[noreturn]] constexpr void
    unreachable([[maybe_unused]] details::with_source_location<std::optional<std::string>> message =
                    std::nullopt) noexcept {
        if (std::is_constant_evaluated()) {
            std::unreachable();
        } else {
#ifdef NDEBUG
            std::unreachable();
#else
            try {
                // 尝试完整记录日志。

                // 获取堆栈信息。
                std::string stack_trace_message;
                try {
                    stack_trace_message = details::get_stack_trace_message();
                } catch (...) {
                    stack_trace_message = "Due to an exception, we lost stack trace message.";
                }

                // 输出信息。
                if (message.value.has_value()) {
                    spdlog::critical(
                        "Control reached an unreachable place at {}:{}:{} (in function :{}):\n"
                        ">> Message: {}\n"
                        "Stack trace:\n"
                        "{}",
                        message.loc.file_name(), message.loc.line(), message.loc.column(),
                        message.loc.function_name(), *message.value, stack_trace_message);
                } else {
                    spdlog::critical(
                        "Control reached an unreachable place at {}:{}:{} (in function :{}):\n"
                        "Stack trace:\n"
                        "{}",
                        message.loc.file_name(), message.loc.line(), message.loc.column(),
                        message.loc.function_name(), stack_trace_message);
                }

                // 及时刷新。
                spdlog::default_logger()->flush();
            } catch (...) {
                // 尝试简单日志。
                try {
                    spdlog::critical("Assertion failed, and error logging threw an exception.\n");
                    spdlog::default_logger()->flush();
                } catch (...) { // NOLINT(bugprone-empty-catch)
                    // 静默退出。
                }
            }

            // 终止程序。
            std::terminate();
#endif // NDEBUG
        }
    }

    /**
     * @brief 带消息的 `std::unreachable`。（执行到正常情况无法到达的位置时的处理函数）
     * @tparam Args 格式化字符串参数类型。
     * @param [in] fmt 格式化字符串。
     * @param [in] args 格式化字符串参数。
     * @details
     * - 当在编译期求值时，宏调用 `std::unreachable()`，**不输出消息**；
     *   当在运行期求值时：
     *   当定义宏 `NDEBUG` 时，函数只调用 `std::unreachable()`，**不输出消息**，若到达 **会导致UB**；
     *   未定义宏 `NDEBUG` 时，会输出消息，行为定义。（调用 `std::terminate()` 终止函数）
     * - 输出格式：
     * ```plain
     * Control reached an unreachable place at <file>:<line>:<column> (in function :<function>):
     * >> Message: <message>
     * Stack trace:
     * frame #0 : <description> in line <line> in file <file>
     * ...
     * ```
     * @warning 此函数会调用 `std::terminate()` / `std::unreachable()`，不会返回，请确认调用处是否无法到达。
     */
    template <typename... Args>
    [[noreturn]] constexpr void
    unreachable([[maybe_unused]] details::with_source_location<std::format_string<Args...>> fmt,
                [[maybe_unused]] Args &&...args) noexcept {
        if (std::is_constant_evaluated()) {
            std::unreachable();
        } else {
#ifdef NDEBUG
            std::unreachable();
#else
            try {
                // 尝试完整信息。
                unreachable(details::with_source_location<std::optional<std::string>>{
                    std::format(fmt.value, std::forward<Args>(args)...), fmt.loc});
            } catch (...) {
                try {
                    // 尝试简短信息。
                    unreachable(details::with_source_location<std::optional<std::string>>{
                        "Due to an exception, we lost unreachable message.\n", fmt.loc});
                } catch (...) {
                    // 静默报错。
                    unreachable(details::with_source_location<std::optional<std::string>>{std::nullopt,
                                                                                          fmt.loc});
                }
            }
#endif // NDEBUG
        }
    }
} // namespace thr

/**
 * @brief 带消息的 `assert`（断言）。
 * @param [in] expr 断言表达式。（需要可以隐式转化为 `bool` 类型或本来就是 `bool` 类型）
 * @param [in] ... 可选的断言失败时输出的消息，支持格式化。
 * @details
 * - 与标准库的 `assert` 行为相同，当定义宏 `NDEBUG` 时不启用，未定义宏 `NDEBUG` 时才启用。
 * - 格式化使用标准库的 `std::format` 格式化。
 * - 输出格式：
 * ```plain
 * Assertion failed at <file>:<line>:<column> (in function :<function>):
 * >> Expression: <expression>
 * >> Message: <message>
 * Stack trace:
 * frame #0 : <description> in line <line> in file <file>
 * ```
 */
#ifdef NDEBUG
#define THR_ASSERT_MSG(expr, ...) [[assume(expr)]]
#else
#define THR_ASSERT_MSG(expr, ...)                                                                       \
    ::thr::details::assert_check(expr, #expr, std::source_location::current() __VA_OPT__(, ) __VA_ARGS__)
#endif // NDEBUG

#endif // THR_BASE_ASSERT_MSG_HPP
