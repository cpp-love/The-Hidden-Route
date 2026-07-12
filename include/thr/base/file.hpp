/**
 * @file file.hpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 声明了一些与文件相关的函数。
 * @version 0.1.0-2
 * @date 2026-07-12
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
     * @retval std::nullopt 获取失败。
     */
    [[nodiscard]] std::optional<std::filesystem::path> get_executable_path() noexcept;

    /**
     * @brief 获取当前可执行文件父文件夹路径。
     * @return std::optional<std::filesystem::path> 可执行文件父文件夹路径。
     * @retval std::nullopt 获取失败。
     */
    [[nodiscard]] std::optional<std::filesystem::path> get_executable_directory() noexcept;

    /**
     * @brief 获取存在的完整路径。
     * @param [in] path 相对路径。
     * @return std::optional<std::filesystem::path> 存在的完整路径。
     * @retval std::nullopt 不存在这样的完整路径。
     * @details 默认相对于当前可执行文件父文件夹，若此路径不存在则尝试相对于工作目录的路径。
     */
    [[nodiscard]] std::optional<std::filesystem::path>
    get_existing_full_path(const std::filesystem::path &path) noexcept;

} // namespace thr

#endif // THR_BASE_FILE_HPP