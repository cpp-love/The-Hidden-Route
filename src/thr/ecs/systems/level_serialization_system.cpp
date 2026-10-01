/**
 * @file level_serialization_system.cpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 实现了序列化迷宫的系统。
 * @version 0.1.0-8
 * @date 2026-10-01
 * 
 * @copyright cpp-love
 * 
 */

#include "thr/ecs/systems/level_serialization_system.hpp"
#include "thr/base/file.hpp"
#include "thr/base/sfml_helper.hpp"
#include "thr/ecs/components/global/game_base.hpp"
#include "thr/ecs/components/level_components.hpp"
#include "thr/ecs/components/maze_components.hpp"
#include "thr/ecs/components/player_components.hpp"
#include <SFML/Graphics/Text.hpp>
#include <entt/entity/registry.hpp>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <optional>
#include <ranges>
#include <string_view>
#include <utility>

namespace thr::ecs {

    nlohmann::json level_serialization_system::serialize_to_json(const entt::registry &registry) {
        nlohmann::json json;

        // serialize segments
        auto           list = registry.view<segment>();
        auto           segments = nlohmann::json::array_t();
        segments.reserve(list.size());
        std::map<entt::entity, std::size_t> list_map;
        for (auto [idx, entity] : list | std::views::enumerate) {
            list_map.try_emplace(entity, idx);
        }
        auto transform_entity = [&](entt::entity entity) -> std::optional<std::size_t> {
            auto iter = list_map.find(entity);
            if (iter == list_map.end()) {
                return {};
            }
            return iter->second;
        };
        for (const auto &[entity, seg] : list.each()) {
            const auto &outline = seg.outline;
            auto        vertexs = nlohmann::json::array_t();
            vertexs.reserve(outline.vertexs.size());
            for (const auto &row : outline.vertexs) {
                auto json_row = nlohmann::json::array_t();
                for (const sf::Vector2f &pos : row) {
                    json_row.emplace_back(pos);
                }
                vertexs.emplace_back(std::move(json_row));
            }
            nlohmann::json outline_json = {{"color", outline.color}, {"vertexs", std::move(vertexs)}};

            segments.push_back({{"prev", seg.prev.and_then(transform_entity)},
                                {"next", seg.next.and_then(transform_entity)},
                                {"start_center", seg.start_center},
                                {"length", seg.length},
                                {"walked_precent", seg.infos.get_current_state().walked_precent},
                                {"dir", direction_to_name(seg.dir)},
                                {"layer", seg.layer},
                                {"tags",
                                 [&] {
                                     const auto *cur_tag = registry.try_get<tag>(entity);
                                     if (cur_tag) {
                                         return cur_tag->tag_ids;
                                     }
                                     return std::set<int>{};
                                 }() | std::ranges::to<std::vector>()},
                                {"outline", outline_json}});
        }
        json["segments"] = std::move(segments);

        // serialize level_script
        const auto *script = registry.ctx().find<level_script>();
        if (script != nullptr) {
            json["level_script"] = script->script_file_name;
        } else {
            json["level_script"] = nullptr;
        }

        // serialize player_infos
        const auto &players = registry.view<player>();
        auto        player_infos = nlohmann::json::array_t();
        for (const auto &[entity, player] : players.each()) {
            player_infos.push_back(
                {{"start_segment_entity", transform_entity(player.start_segment_entity).value()},
                 {"end_segment_entity", transform_entity(player.end_segment_entity).value()},
                 {"color", player.color}});
        }
        json["player_infos"] = std::move(player_infos);

        return json;
    }

    void level_serialization_system::deserialize_from_json(entt::registry       &registry,
                                                           const nlohmann::json &json) {
        // deserialize segments
        const auto               &segments_json = json.at("segments");
        std::vector<entt::entity> segment_entities(segments_json.size());
        registry.create(segment_entities.begin(), segment_entities.end());

        for (const auto &[seg_json, entity] : std::views::zip(segments_json, segment_entities)) {
            segment seg{.start_center = seg_json.at("start_center"),
                        .length = seg_json.at("length"),
                        .dir = name_to_direction(seg_json.at("dir").get<std::string_view>()),
                        .layer = seg_json.value("layer", 0)};

            auto    prev = seg_json.value("prev", nlohmann::json());
            if (!prev.is_null()) {
                seg.prev = segment_entities.at(prev);
            } else {
                entt::entity node_entity = registry.create();
                node         node{.position = seg.start_center, .segment_entity = entity};
                registry.emplace<struct node>(node_entity, node);
            }
            auto next = seg_json.value("next", nlohmann::json());
            if (!next.is_null()) {
                seg.next = segment_entities.at(next);
            } else {
                entt::entity node_entity = registry.create();
                node node{.position = seg.start_center + direction_to_vector2f(seg.dir, seg.length),
                          .segment_entity = entity};
                registry.emplace<struct node>(node_entity, node);
            }

            auto tags = seg_json.value("tags", std::vector<int>());
            if (!tags.empty()) {
                registry.emplace<tag>(entity, std::set<int>(std::from_range, tags));
            }

            if (auto outline_it = seg_json.find("outline"); outline_it != seg_json.end()) {
                const auto &outline_json = *outline_it;
                seg.outline.color = outline_json.value("color", nlohmann::json("white"));
                if (auto vertexs_it = outline_json.find("vertexs"); vertexs_it != outline_json.end()) {
                    seg.outline.vertexs.reserve(vertexs_it->size());
                    for (const auto &json_row : *vertexs_it) {
                        std::vector<sf::Vector2f> row;
                        row.reserve(json_row.size());
                        for (const auto &pos : json_row) {
                            row.emplace_back(pos);
                        }
                        seg.outline.vertexs.emplace_back(std::move(row));
                    }
                }
            }

            registry.emplace<segment>(entity, seg);
        }

        // deserialize level_script
        json.value("level_script", std::optional<std::string_view>())
            .transform([&](std::string_view script) {
                registry.ctx().emplace<level_script>(
                    get_existing_full_path(std::filesystem::path("assets/lua_scripts") / script).value(),
                    std::string(script));
                return 0;
            });

        // deserialize player_infos
        const auto &player_infos_json = json.at("player_infos");
        for (const auto &player_info_json : player_infos_json) {
            auto start_segment_entity = segment_entities.at(player_info_json.at("start_segment_entity"));
            auto end_segment_entity = segment_entities.at(player_info_json.at("end_segment_entity"));
            sf::Color    color = player_info_json.at("color");

            entt::entity player_entity = registry.create();
            {
                player player = {.color = color,
                                 .start_segment_entity = start_segment_entity,
                                 .end_segment_entity = end_segment_entity,
                                 .statuses{{thr::ecs::player::on_ground{start_segment_entity}}}};
                registry.emplace<ecs::player>(player_entity, std::move(player));
            }
            registry.patch<thr::ecs::segment>(start_segment_entity, [&](thr::ecs::segment &seg) {
                seg.infos.get_current_state_modifiable().current_walking_entity = player_entity;
            });

            auto create_endpoint_label = [&](std::string_view text, sf::Vector2f position) {
                constexpr unsigned int character_size = 10;
                sf::Text label{configs::singleton().get_sfml_font(),
                               sf::String::fromUtf8(text.begin(), text.end()), character_size};
                label.setOrigin(label.getLocalBounds().getCenter());
                label.setPosition(position);
                label.setFillColor(color);
                registry.emplace<sf::Text>(registry.create(), std::move(label));
            };

            create_endpoint_label("始", registry.get<segment>(start_segment_entity).start_center);
            create_endpoint_label("终", registry.get<segment>(end_segment_entity).get_end_center());
        }
    }

} // namespace thr::ecs