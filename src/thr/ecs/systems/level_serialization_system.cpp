/**
 * @file level_serialization_system.cpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 实现了序列化迷宫的系统。
 * @version 0.1.0-4
 * @date 2026-07-12
 * 
 * @copyright cpp-love
 * 
 */

#include "thr/ecs/systems/level_serialization_system.hpp"
#include "thr/base/file.hpp"
#include "thr/base/sfml_helper.hpp"
#include "thr/ecs/components/level_components.hpp"
#include "thr/ecs/components/maze_components.hpp"
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

        // serialize line_strips
        const auto    &lines = registry.ctx().get<line_strips>();
        json["line_strips"] = {{"color", lines.color}, {"width", lines.width}};
        auto vertexs = nlohmann::json::array_t();
        vertexs.reserve(lines.vertexs.size());
        for (const auto &row : lines.vertexs) {
            auto json_row = nlohmann::json::array_t();
            for (const sf::Vector2f &pos : row) { json_row.emplace_back(pos); }
            vertexs.emplace_back(std::move(json_row));
        }
        json["line_strips"]["vertexs"] = std::move(vertexs);

        // serialize segments
        auto list = registry.view<segment>();
        auto segments = nlohmann::json::array_t();
        segments.reserve(list.size());
        std::map<entt::entity, std::size_t> list_map;
        for (auto [idx, entity] : list | std::views::enumerate) { list_map.try_emplace(entity, idx); }
        for (entt::entity entity : list) {
            const auto &seg = registry.get<segment>(entity);
            auto        transform_entity = [&](entt::entity entity) -> std::optional<std::size_t> {
                auto iter = list_map.find(entity);
                if (iter == list_map.end()) {
                    return {};
                }
                return iter->second;
            };
            segments.push_back({{"prev", seg.prev.and_then(transform_entity)},
                                {"next", seg.next.and_then(transform_entity)},
                                {"start_center", seg.start_center},
                                {"length", seg.length},
                                {"walked_precent", seg.walked_precent},
                                {"dir", direction_to_name(seg.dir)},
                                {"tags", [&] {
                                     const auto *cur_tag = registry.try_get<tag>(entity);
                                     if (cur_tag) {
                                         return cur_tag->tag_ids;
                                     }
                                     return std::set<int>{};
                                 }() | std::ranges::to<std::vector>()}});
        }
        json["segments"] = std::move(segments);

        // serialize level_script
        const auto *script = registry.ctx().find<level_script>();
        if (script != nullptr) {
            json["level_script"] = script->script_file_name;
        } else {
            json["level_script"] = nullptr;
        }

        // serialize level_info
        const auto &level_info = registry.ctx().get<thr::ecs::level_info>();
        json["level_info"] = {{"start_segment_entity", level_info.start_segment_entity},
                              {"end_segment_entity", level_info.end_segment_entity}};

        return json;
    }

    void level_serialization_system::deserialize_from_json(entt::registry       &registry,
                                                           const nlohmann::json &json) {

        // deserialize line_strips
        if (auto line_strips_it = json.find("line_strips"); line_strips_it != json.end()) {
            const auto &json_line_strips = *line_strips_it;
            line_strips line_strips;
            line_strips.color = json_line_strips.value("color", nlohmann::json("white"));
            line_strips.width = json_line_strips.value("width", line_strips::default_width);
            if (auto vertexs_it = json_line_strips.find("vertexs");
                vertexs_it != json_line_strips.end()) {
                line_strips.vertexs.reserve(vertexs_it->size());
                for (const auto &json_row : *vertexs_it) {
                    std::vector<sf::Vector2f> row;
                    row.reserve(json_row.size());
                    for (const auto &pos : json_row) { row.emplace_back(pos); }
                    line_strips.vertexs.emplace_back(std::move(row));
                }
            }
            registry.ctx().insert_or_assign(line_strips);
        } else {
            registry.ctx().insert_or_assign(line_strips{});
        }

        // deserialize segments
        const auto               &segments_json = json.at("segments");
        std::vector<entt::entity> segment_entities(segments_json.size());
        registry.create(segment_entities.begin(), segment_entities.end());

        for (const auto &[seg_json, entity] : std::views::zip(segments_json, segment_entities)) {
            segment seg{.start_center = seg_json.at("start_center"),
                        .length = seg_json.at("length"),
                        .walked_precent = seg_json.value("walked_precent", 0.f),
                        .dir = name_to_direction(seg_json.at("dir").get<std::string_view>())};
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

        // deserialize level_info
        const auto &level_info_json = json.at("level_info");
        level_info  level_info{
            .start_segment_entity = segment_entities.at(level_info_json.at("start_segment_entity")),
            .end_segment_entity = segment_entities.at(level_info_json.at("end_segment_entity"))};
        registry.ctx().emplace<struct level_info>(level_info);
    }

} // namespace thr::ecs