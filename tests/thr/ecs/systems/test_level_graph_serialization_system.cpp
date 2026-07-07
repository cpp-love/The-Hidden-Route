/**
 * @file test_level_graph_serialization_system.cpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief `thr::ecs::level_graph_serialization_system` 的回归测试。
 * @version 0.1.0-1
 * @date 2026-07-07
 *
 * @copyright cpp-love
 *
 */

#undef NDEBUG
#include "thr/base/assert_msg.hpp"
#include "thr/ecs/components/level_graph_components.hpp"
#include "thr/ecs/systems/level_graph_serialization_system.hpp"
#include <entt/entity/registry.hpp>
#include <nlohmann/json.hpp>

int main() {
    entt::registry     registry;
    const entt::entity start_entity = registry.create();
    const entt::entity next_entity = registry.create();

    registry.emplace<thr::ecs::level_node>(start_entity, std::vector<entt::entity>{next_entity},
                                           sf::Vector2f{100.f, 120.f}, false);
    registry.emplace<thr::ecs::level_node>(next_entity, std::vector<entt::entity>{},
                                           sf::Vector2f{220.f, 180.f}, true);

    const nlohmann::json json = thr::ecs::level_graph_serialization_system::serialize_to_json(registry);
    THR_ASSERT_MSG(json.contains("level_nodes"));
    THR_ASSERT_MSG(json["level_nodes"].size() == 2U);

    bool saw_linked_node = false;
    bool saw_leaf_node = false;
    for (const auto &node_json : json["level_nodes"]) {
        THR_ASSERT_MSG(node_json.contains("position"));
        THR_ASSERT_MSG(node_json["position"].is_object());
        THR_ASSERT_MSG(node_json["position"].contains("x"));
        THR_ASSERT_MSG(node_json["position"].contains("y"));
        if (node_json["relative_entities"].size() == 1U) {
            saw_linked_node = true;
        } else if (node_json["relative_entities"].empty()) {
            saw_leaf_node = true;
        }
    }
    THR_ASSERT_MSG(saw_linked_node && saw_leaf_node);

    entt::registry restored;
    thr::ecs::level_graph_serialization_system::deserialize_from_json(restored, json);

    auto view = restored.view<thr::ecs::level_node>();
    THR_ASSERT_MSG(view.size() == 2U);

    bool saw_start = false;
    bool saw_next = false;
    for (const entt::entity entity : view) {
        const auto &node = restored.get<thr::ecs::level_node>(entity);
        if (node.position == sf::Vector2f{100.f, 120.f}) {
            saw_start = true;
            THR_ASSERT_MSG(!node.locked);
            THR_ASSERT_MSG(node.relative_entities.size() == 1U);
            THR_ASSERT_MSG(node.relative_entities.front() != entt::null);
            const auto &linked = restored.get<thr::ecs::level_node>(node.relative_entities.front());
            THR_ASSERT_MSG((linked.position == sf::Vector2f{220.f, 180.f}));
        } else if (node.position == sf::Vector2f{220.f, 180.f}) {
            saw_next = true;
            THR_ASSERT_MSG(node.locked);
            THR_ASSERT_MSG(node.relative_entities.empty());
        }
    }

    THR_ASSERT_MSG(saw_start && saw_next);
    return 0;
}
