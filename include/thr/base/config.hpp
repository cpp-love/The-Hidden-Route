/**
 * @file config.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief thr项目的基本配置信息
 * @version ..-2
 * @date 2025-08-16
 * 
 * @copyright cpp-love
 * 
 */

#ifndef THR_BASE_CONFIG_HPP
#define THR_BASE_CONFIG_HPP

#include <cstdint>

namespace thr {

    using version_type = std::uint16_t;               ///< thr版本类型
    using level_identifier_type = std::uint16_t;      ///< 关卡标识符类型
    constexpr version_type version_major_offset = 11; ///< thr主版本偏移量
    constexpr version_type version_major = 0;         ///< thr主版本号
    constexpr version_type version_minor_offset = 6;  ///< thr次版本偏移量
    constexpr version_type version_minor = 1;         ///< thr次版本号
    constexpr version_type version_patch_offset = 0;  ///< thr修订版本偏移量
    constexpr version_type version_patch = 0;         ///< thr修订版本号
    constexpr version_type version = (version_major << version_major_offset)
                                     + (version_minor << version_minor_offset)
                                     + (version_patch << version_patch_offset); ///< thr版本号
} // namespace thr

#endif // THR_BASE_CONFIG_HPP
