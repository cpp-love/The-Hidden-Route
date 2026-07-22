/**
 * @file lua_manager.hpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 定义了与 Lua 沟通的管理类。
 * @version 0.1.0-1
 * @date 2026-07-08
 * 
 * @copyright cpp-love
 * 
 */

#ifndef THR_ECS_LUA_BINDINGS_LUA_MANAGER_HPP
#define THR_ECS_LUA_BINDINGS_LUA_MANAGER_HPP

#include "thr/ecs/systems/level_serialization_system.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <entt/entity/fwd.hpp>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <iostream>
#include <nlohmann/json.hpp>
#include <optional>
#include <sol/error.hpp>
#include <sol/raii.hpp>
#include <sol/sol.hpp>
#include <string>
#include <utility>

namespace thr::ecs::lua_bindings {

    /// @brief 与 Lua 沟通的管理类。
    class lua_manager {
      private:
        sol::state                             m_lua;      ///< Lua 状态机。
        std::reference_wrapper<entt::registry> m_registry; ///< 注册表。

      public:
        /**
         * @brief 构建一个 lua manager 对象。
         * @param [in] registry 注册表。
         */
        explicit lua_manager(std::reference_wrapper<entt::registry> registry) : m_registry(registry) {
            init();
        }

        /**
         * @brief 从文件加载并执行 Lua 脚本。
         * @param [in] path Lua 脚本路径。
         * @return std::optional<std::string> 可能的错误。
         */
        std::optional<std::string> load_and_run_script_file(const std::filesystem::path &path) noexcept {
            auto result = m_lua.safe_script_file(path.generic_string());
            if (!result.valid()) {
                return result.get<sol::error>().what();
            }
            return std::nullopt;
        }

        /**
         * @brief 调用 Lua 函数。
         * @tparam Args 参数类型。
         * @param [in] function_name 函数名。
         * @param [in] args 参数。
         * @return std::optional<std::string> 可能的错误。
         */
        template <typename... Args>
        std::optional<std::string> call_function(std::string_view function_name,
                                                 Args &&...args) noexcept {
            sol::protected_function function = m_lua[function_name];
            if (function.valid()) {
                sol::protected_function_result result = function(std::forward<Args>(args)...);
                if (!result.valid()) {
                    return result.get<sol::error>().what();
                }
            }
            return std::nullopt;
        }

      private:
        /// @brief 统一的初始化函数。
        void init();
    };

} // namespace thr::ecs::lua_bindings

#endif // THR_ECS_LUA_BINDINGS_LUA_MANAGER_HPP