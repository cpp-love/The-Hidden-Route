/**
 * @file level_graph_serialization_system.cpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 定义了序列化关卡图的系统。
 * @version 0.1.0-1
 * @date 2026-07-04
 * 
 * @copyright cpp-love
 * 
 */

#include "thr/ecs/systems/level_graph_serialization_system.hpp"
#include "thr/base/sfml_helper.hpp"
#include "thr/ecs/components/level_graph_components.hpp"
#include <cstddef>
#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <ranges>
#include <vector>

namespace thr::ecs {

    nlohmann::json level_graph_serialization_system::serialize_to_json(const entt::registry &registry) {
        nlohmann::json json;

        auto           list = registry.view<level_node>();
        auto           nodes = nlohmann::json::array_t();
        nodes.reserve(list.size());
        std::map<entt::entity, std::size_t> list_map;
        for (auto [idx, entity] : list | std::views::enumerate) { list_map.try_emplace(entity, idx); }
        for (entt::entity entity : list) {
            const auto &node = registry.get<level_node>(entity);
            nodes.push_back(
                {{"relative_entities",
                  node.relative_entities
                      | std::views::transform([&](entt::entity entity) -> std::optional<std::size_t> {
                            auto iter = list_map.find(entity);
                            if (iter == list_map.end()) {
                                return {};
                            }
                            return iter->second;
                        })
                      | std::ranges::to<std::vector>()},
                 {"position", node.position},
                 {"locked", node.locked}});
        }
        json["level_nodes"] = nodes;

        return json;
    }
    void level_graph_serialization_system::deserialize_from_json(entt::registry       &registry,
                                                                 const nlohmann::json &json) {
        auto                      nodes_json = json.value("level_nodes", nlohmann::json::array_t());
        std::vector<entt::entity> node_entities(nodes_json.size());
        registry.create(node_entities.begin(), node_entities.end());
        for (const auto &[node_json, entity] : std::views::zip(nodes_json, node_entities)) {
            registry.emplace<level_node>(entity,
                                         node_json.value("relative_entities", std::vector<std::size_t>())
                                             | std::views::transform([&](std::size_t entity_index) {
                                                   return node_entities.at(entity_index);
                                               })
                                             | std::ranges::to<std::vector>(),
                                         node_json.at("position"), node_json.value("locked", true));
        }
    }

} // namespace thr::ecs