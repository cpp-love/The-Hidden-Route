/**
 * @file level_components.hpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 定义了关卡相关的组件。
 * @version 0.1.0-1
 * @date 2026-07-09
 * 
 * @copyright cpp-love
 * 
 */

#ifndef THR_ECS_COMPONENTS_LEVEL_COMPONENTS_HPP
#define THR_ECS_COMPONENTS_LEVEL_COMPONENTS_HPP

#include <entt/entity/entity.hpp>
#include <filesystem>
#include <set>

namespace thr::ecs {

    /// @brief 关卡的基本信息。
    struct level_info {
        entt::entity start_segment_entity{entt::null}; ///< 起始段落对应的实体。
        entt::entity end_segment_entity{entt::null};   ///< 结束段落对应的实体。
    };

    /// @brief 关卡对应的脚本。
    struct level_script {
        std::filesystem::path script_path; ///< 关卡脚本的路径。
        std::string script_file_name;      ///< 关卡脚本的文件名（相对于 `assets/lua_scripts/` 目录）。
    };

    /// @brief 特殊标签。
    struct tag {
        std::set<int> tag_ids; ///< 标签的所有编号。
    };

} // namespace thr::ecs

#endif // THR_ECS_COMPONENTS_LEVEL_COMPONENTS_HPP