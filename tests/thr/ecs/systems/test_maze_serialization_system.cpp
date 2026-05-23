/**
 * @file test_maze_serialization_system.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 添加了 `thr::ecs::maze_serialization_system` 的测试用例和使用示例。
 * @version 0.1.0-1
 * @date 2026-05-23
 * 
 * @copyright cpp-love
 * 
 */

#undef NDEBUG
#include "thr/ecs/systems/maze_serialization_system.hpp"
#include <entt/entity/registry.hpp>
#include <format>
#include <fstream>
#include <nlohmann/json.hpp>
#include <print>
#include <thr/base/assert_msg.hpp>

int main() {

    entt::registry registry;
    nlohmann::json json;
    std::ifstream  fin("maze.json");
    if (!fin.is_open()) {
        std::println("Error: could not open the file maze.json");
        return -1;
    }
    fin >> json;
    thr::ecs::maze_serialization_system::deserialize_from_json(registry, json);
    nlohmann::json json2 = thr::ecs::maze_serialization_system::serialize_to_json(registry);
    std::println("json: {}", nlohmann::to_string(json));
    std::println("json2: {}", nlohmann::to_string(json2));

    return 0;
}