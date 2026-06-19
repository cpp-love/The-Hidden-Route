/**
 * @file level_serialization_system.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 实现了序列化迷宫的系统。
 * @version 0.1.0-2
 * @date 2026-06-19
 * 
 * @copyright cpp-love
 * 
 */

#include "thr/ecs/systems/level_serialization_system.hpp"
#include "thr/ecs/components/maze_components.hpp"
#include "thr/ecs/components/player_components.hpp"
#include <entt/entity/registry.hpp>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <string>
#include <utility>

namespace thr::ecs {

    nlohmann::json level_serialization_system::serialize_to_json(const entt::registry &registry) {
        nlohmann::json json;

        // serialize line_strips
        const auto    &lines = registry.ctx().get<line_strips>();
        json["line_strips"] = {{"color",
                                [](sf::Color color) {
                                    /// @todo 实现更多颜色
                                    if (color == sf::Color::Yellow) {
                                        return "yellow";
                                    }
                                    if (color == sf::Color::White) {
                                        return "white";
                                    }
                                    if (color == sf::Color::Transparent) {
                                        return "transparent";
                                    }
                                    if (color == sf::Color::Red) {
                                        return "red";
                                    }
                                    if (color == sf::Color::Magenta) {
                                        return "magenta";
                                    }
                                    if (color == sf::Color::Green) {
                                        return "green";
                                    }
                                    if (color == sf::Color::Cyan) {
                                        return "cyan";
                                    }
                                    if (color == sf::Color::Blue) {
                                        return "blue";
                                    }
                                    if (color == sf::Color::Black) {
                                        return "black";
                                    }
                                    throw std::invalid_argument(
                                        "in thr::ecs::level_serialization_system::serialize_to_json(): "
                                        "serialize meets an unsupported color");
                                }(lines.color)},
                               {"width", lines.width}};
        auto vertexs = nlohmann::json::array_t();
        vertexs.reserve(lines.vertexs.size());
        for (const auto &row : lines.vertexs) {
            auto json_row = nlohmann::json::array_t();
            for (const sf::Vector2f &pos : row) { json_row.push_back({{"x", pos.x}, {"y", pos.y}}); }
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
                {{"prev",
                  seg.prev
                      .transform([&](entt::entity entity) {
                          return nlohmann::json(list_map.find(entity)->second);
                      })
                      .or_else([] -> std::optional<nlohmann::json> { return nlohmann::json(); })},
                 {"next",
                  seg.next
                      .transform([&](entt::entity entity) {
                          return nlohmann::json(list_map.find(entity)->second);
                      })
                      .or_else([] -> std::optional<nlohmann::json> { return nlohmann::json(); })},
                 {"start_center", {{"x", seg.start_center.x}, {"y", seg.start_center.y}}},
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
            line_strips.color = [](std::string_view color) {
                /// @todo 实现更多颜色
                if (color == "yellow") {
                    return sf::Color::Yellow;
                }
                if (color == "white") {
                    return sf::Color::White;
                }
                if (color == "transparent") {
                    return sf::Color::Transparent;
                }
                if (color == "red") {
                    return sf::Color::Red;
                }
                if (color == "magenta") {
                    return sf::Color::Magenta;
                }
                if (color == "green") {
                    return sf::Color::Green;
                }
                if (color == "cyan") {
                    return sf::Color::Cyan;
                }
                if (color == "blue") {
                    return sf::Color::Blue;
                }
                if (color == "black") {
                    return sf::Color::Black;
                }
                throw std::invalid_argument(
                    "in thr::ecs::level_serialization_system::serialize_to_json(): "
                    "serialize meets an unsupported color");
            }(json_line_strips.value("color", "white"));
            line_strips.width = json_line_strips.value("width", line_strips::default_width);
            if (auto vertexs_it = json_line_strips.find("vertexs");
                vertexs_it != json_line_strips.end()) {
                line_strips.vertexs.reserve(vertexs_it->size());
                for (const auto &json_row : *vertexs_it) {
                    std::vector<sf::Vector2f> row;
                    row.reserve(json_row.size());
                    for (const auto &pos : json_row) { row.emplace_back(pos["x"], pos["y"]); }
                    line_strips.vertexs.emplace_back(std::move(row));
                }
            }
            registry.ctx().insert_or_assign(line_strips);
        } else {
            registry.ctx().insert_or_assign(line_strips{});
        }

        // deserialize segments
        const auto               &segments_json = json["segments"];
        std::vector<entt::entity> segment_entities(segments_json.size());
        registry.create(segment_entities.begin(), segment_entities.end());

        for (const auto &[json_seg, entity] : std::views::zip(segments_json, segment_entities)) {
            segment seg;
            seg.start_center = {json_seg["start_center"]["x"], json_seg["start_center"]["y"]};
            seg.length = json_seg["length"];
            seg.walked_precent = json_seg.value("walked_precent", 0.f);
            seg.dir = name_to_direction(json_seg["dir"].get<std::string>());
            auto prev = json_seg.value("prev", nlohmann::json());
            if (!prev.is_null()) {
                seg.prev = segment_entities.at(prev);
            } else {
                entt::entity node_entity = registry.create();
                node         node{.position = seg.start_center, .segment_entity = entity};
                registry.emplace<struct node>(node_entity, node);
            }
            auto next = json_seg.value("next", nlohmann::json());
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
        const auto &level_info_json = json["level_info"];
        level_info  level_info{
             .start_segment_entity = segment_entities.at(level_info_json["start_segment_entity"]),
             .end_segment_entity = segment_entities.at(level_info_json["end_segment_entity"])};
        registry.ctx().emplace<struct level_info>(level_info);
    }

} // namespace thr::ecs