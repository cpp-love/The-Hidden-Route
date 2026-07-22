/**
 * @file lua_manager.cpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 实现了与 Lua 沟通的管理类。
 * @version 0.1.0-2
 * @date 2026-07-22
 * 
 * @copyright cpp-love
 * 
 */

#include "thr/ecs/lua_bindings/lua_manager.hpp"
#include "thr/base/sfml_helper.hpp"
#include "thr/ecs/lua_bindings/entity_wrapper.hpp"
#include "thr/ecs/lua_bindings/game_api.hpp"
#include <SFML/System/Vector2.hpp>
#include <sol/forward.hpp>
#include <sol/sol.hpp>
#include <sol/types.hpp>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>

namespace thr::ecs::lua_bindings {

    void lua_manager::init() {
        // 默认初始化。
        m_lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::table, sol::lib::math,
                             sol::lib::string);

        // 辅助函数。
        auto generate_message = [tostring =
                                     m_lua["tostring"].get<sol::function>()](sol::variadic_args args) {
            std::string msg = "[from Lua] ";
            bool        is_first = true;
            for (const auto &var : args) {
                if (!is_first) {
                    msg += '\t';
                }
                msg += tostring(var);
                is_first = false;
            }
            return msg;
        };

        // 注册一系列仿 spdlog 式的输出函数。
        sol::table logger = m_lua.create_table("Logger");
        logger.set_function("trace", [generate_message](sol::variadic_args args) {
            spdlog::trace(generate_message(args));
        });
        logger.set_function("debug", [generate_message](sol::variadic_args args) {
            spdlog::debug(generate_message(args));
        });
        logger.set_function("info", [generate_message](sol::variadic_args args) {
            spdlog::info(generate_message(args));
        });
        logger.set_function("warn", [generate_message](sol::variadic_args args) {
            spdlog::warn(generate_message(args));
        });
        logger.set_function("error", [generate_message](sol::variadic_args args) {
            spdlog::error(generate_message(args));
        });
        logger.set_function("critical", [generate_message](sol::variadic_args args) {
            spdlog::critical(generate_message(args));
        });

        // 注册 Vector2f 到 Lua。
        m_lua.new_usertype<sf::Vector2f>(
            "Vector2", sol::constructors<sf::Vector2f(), sf::Vector2f(float, float)>(),

            "x", &sf::Vector2f::x, "y", &sf::Vector2f::y,

            sol::meta_function::unary_minus, [](const sf::Vector2f &lhs) { return -lhs; },

            sol::meta_function::addition,
            [](const sf::Vector2f &lhs, const sf::Vector2f &rhs) { return lhs + rhs; },

            sol::meta_function::subtraction,
            [](const sf::Vector2f &lhs, const sf::Vector2f &rhs) { return lhs - rhs; },

            sol::meta_function::multiplication,
            sol::overload([](const sf::Vector2f &lhs, float rhs) { return lhs * rhs; },
                          [](float lhs, const sf::Vector2f &rhs) { return lhs * rhs; }),

            sol::meta_function::division, [](const sf::Vector2f &lhs, float rhs) { return lhs / rhs; },

            sol::meta_function::equal_to,
            [](const sf::Vector2f &lhs, const sf::Vector2f &rhs) { return lhs == rhs; },

            sol::meta_function::to_string,
            [](const sf::Vector2f &lhs) { return std::format("{}", lhs); });

        // 注册 Entity 包装器到 Lua。
        m_lua.new_usertype<entity_wrapper>(
            "Entity", sol::no_constructor,

            "valid", &entity_wrapper::valid,

            "destroy", &entity_wrapper::destroy,

            "add_tag", &entity_wrapper::add_tag,

            "remove_tag", &entity_wrapper::remove_tag,

            "query_tag", &entity_wrapper::query_tag,

            sol::meta_function::equal_to,
            [](const entity_wrapper &lhs, const entity_wrapper &rhs) { return lhs == rhs; });

        // 注册 Game API 到 Lua。
        m_lua.new_usertype<game_api>(
            "Game", sol::no_constructor,

            "create_entity", &game_api::create_entity,

            "query_entities_with_tag", &game_api::query_entities_with_tag,

            "add_text",
            sol::overload([&](game_api &game, std::string_view text,
                              sf::Vector2f position) { return game.add_text(text, position); },
                          [&](game_api &game, std::string_view text, sf::Vector2f position,
                              unsigned int character_size) {
                              return game.add_text(text, position, character_size);
                          }));

        // 将一个 `game` 全局变量放入 Lua，供脚本直接使用。
        m_lua["game"] = game_api(m_registry);
    }

} // namespace thr::ecs::lua_bindings