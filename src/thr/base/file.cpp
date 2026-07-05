/**
 * @file file.cpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 实现了一些与文件相关的函数。
 * @version 0.1.0-1
 * @date 2026-06-19
 * 
 * @copyright cpp-love
 * 
 */

#include "thr/base/file.hpp"
#include <filesystem>
#include <vector>

#ifdef _WIN32
#include <windows.h>

#elif defined(__APPLE__)
#include <cstdint>
#include <mach-o/dyld.h>
#include <system_error>

#elif defined(__linux__)
#include <cerrno>
#include <cstddef>
#include <unistd.h>
#endif

namespace thr {

    std::optional<std::filesystem::path> get_executable_path() noexcept {
        constexpr int max_attempts = 10; //< 最大尝试次数。
#ifdef _WIN32
        std::vector<wchar_t> buffer(MAX_PATH);
        for (int i = 0; i < max_attempts; ++i) {
            DWORD len = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
            if (len == 0) {
                return std::nullopt; // 调用失败
            }
            if (len < buffer.size()) {
                return std::filesystem::path(buffer.data());
            }
            // 缓冲区不足，扩容重试
            buffer.resize(buffer.size() * 2);
        }
#elif defined(__APPLE__)
        std::uint32_t     size = 1024; //< 文件大小。
        std::vector<char> buffer(size);
        for (int i = 0; i < max_attempts; ++i) {
            int result = _NSGetExecutablePath(buffer.data(), &size);
            if (result == 0) {
                std::error_code       ec;
                std::filesystem::path real_path = std::filesystem::canonical(buffer.data(), ec);
                if (!ec) {
                    return real_path;
                }
                // 解析失败时，回退到原始路径
                return std::filesystem::path(buffer.data());
            }
            // 返回 -1 表示缓冲区不足，size 已被设置为所需大小
            buffer.resize(size);
        }
#else // Linux / 其他类 Unix
        std::size_t       size = 1024; //< 文件大小。
        std::vector<char> buffer(size);
        for (int i = 0; i < max_attempts; ++i) {
            ssize_t len = readlink("/proc/self/exe", buffer.data(), size - 1);
            if (len == -1) {
                if (errno == ENAMETOOLONG) {
                    size *= 2;
                    buffer.resize(size);
                    continue;
                }
                return std::nullopt; // 其他错误
            }
            buffer[len] = '\0';
            return std::filesystem::path(buffer.data());
        }
#endif
        return std::nullopt;
    }

    std::optional<std::filesystem::path> get_executable_directory() noexcept {
        auto path = get_executable_path();
        return path.transform([](std::filesystem::path path) { return path.parent_path(); });
    }

} // namespace thr