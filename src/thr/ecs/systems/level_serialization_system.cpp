/**
 * @file level_serialization_system.cpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 实现了序列化迷宫的系统。
 * @version 0.1.0-3
 * @date 2026-07-06
 * 
 * @copyright cpp-love
 * 
 */

#include "thr/ecs/systems/level_serialization_system.hpp"
#include "thr/base/sfml_helper.hpp"
#include "thr/ecs/components/maze_components.hpp"
#include "thr/ecs/components/player_components.hpp"
#include <entt/entity/registry.hpp>
#include <nlohmann/json.hpp>
#include <optional>
#include <ranges>
#include <string>
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
            segments.push_back(
                {{"prev", seg.prev.and_then([&](entt::entity entity) -> std::optional<std::size_t> {
                      auto it = list_map.find(entity);
                      if (it == list_map.end()) {
                          return {};
                      }
                      return it->second;
                  })},
                 {"next", seg.next.and_then([&](entt::entity entity) -> std::optional<std::size_t> {
                      auto it = list_map.find(entity);
                      if (it == list_map.end()) {
                          return {};
                      }
                      return it->second;
                  })},
                 {"start_center", seg.start_center},
                 {"length", seg.length},
                 {"walked_precent", seg.walked_precent},
                 {"dir", direction_to_name(seg.dir)}});
        }
        json["segments"] = std::move(segments);

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
            segment seg;
            seg.start_center = seg_json.at("start_center");
            seg.length = seg_json.at("length");
            seg.walked_precent = seg_json.value("walked_precent", 0.f);
            seg.dir = name_to_direction(seg_json.at("dir").get<std::string>());
            auto prev = seg_json.value("prev", nlohmann::json());
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

            registry.emplace<segment>(entity, seg);
        }

        // deserialize level_info
        const auto &level_info_json = json.at("level_info");
        level_info  level_info{
            .start_segment_entity = segment_entities.at(level_info_json.at("start_segment_entity")),
            .end_segment_entity = segment_entities.at(level_info_json.at("end_segment_entity"))};
        registry.ctx().emplace<struct level_info>(level_info);
    }

} // namespace thr::ecs