/**
 * @file file.hpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 声明了一些与文件相关的函数。
 * @version 0.1.0-1
 * @date 2026-06-19
 * 
 * @copyright cpp-love
 * 
 */

#ifndef THR_BASE_FILE_HPP
#define THR_BASE_FILE_HPP

#include <filesystem>
#include <optional>

namespace thr {

    /**
     * @brief 获取当前可执行文件路径。
     * @return std::optional<std::filesystem::path> 可执行文件路径。
     */
    std::optional<std::filesystem::path> get_executable_path() noexcept;

    /**
     * @brief 获取当前可执行文件父文件夹路径。
     * @return std::optional<std::filesystem::path> 可执行文件父文件夹路径。
     */
    std::optional<std::filesystem::path> get_executable_directory() noexcept;

} // namespace thr

#endif // THR_BASE_FILE_HPP