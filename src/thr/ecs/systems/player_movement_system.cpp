/**
 * @file player_movement_system.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 定义了玩家移动系统。
 * @version 0.1.0-1
 * @date 2026-05-30
 * 
 * @copyright cpp-love
 * 
 */

#include "thr/ecs/systems/player_movement_system.hpp"
#include "thr/base/assert_msg.hpp"
#include "thr/base/floating_point_compare.hpp"
#include "thr/base/overload.hpp"
#include "thr/ecs/components/global/game_base.hpp"
#include "thr/ecs/components/maze_components.hpp"
#include "thr/ecs/components/player_components.hpp"
#include "thr/ecs/systems/global/scene_system.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <entt/entt.hpp>

namespace thr::ecs {

    void player_movement_system::update(entt::registry &registry, entt::entity player_entity,
                                        float delta_length, direction dir) noexcept {
        auto       &turnings = registry.get<turning_history>(player_entity).turnings;
        const auto *on_ground = registry.try_get<player_on_ground>(player_entity);
        const auto *under_ground = registry.try_get<player_under_ground>(player_entity);
        if (on_ground != nullptr) {
            // 地上模式。
            THR_ASSERT_MSG(under_ground == nullptr, "实体错误地同时拥有 `thr::ecs::player_on_ground` 和 "
                                                    "`thr::ecs::player_under_ground` 组件。");
            entt::entity seg_entity = on_ground->segment_entity;
            const auto  &seg = registry.get<segment>(seg_entity);
            if (no_nan_inf_f{seg.walked_precent} == no_nan_inf_f{1}) {
                // 走到底了。
                if (negate_direction(dir) == seg.dir) {
                    // 往回走。
                    registry.patch<segment>(seg_entity, [&](segment &seg) {
                        seg.walked_precent -= delta_length / seg.length;
                        seg.wrap_walked_precent();
                    });
                } else {
                    // 继续。
                    seg.next
                        .transform([&](entt::entity next_entity) {
                            // 尝试进入下一段路。
                            const auto &next_seg = registry.get<segment>(next_entity);
                            THR_ASSERT_MSG(next_seg.walked_precent == 0, "下一段路错误地已经被走过了");
                            if (dir == next_seg.dir) {
                                registry.patch<segment>(next_entity, [&](segment &next_seg) {
                                    next_seg.walked_precent = delta_length / seg.length;
                                    next_seg.wrap_walked_precent();
                                });
                                turnings.emplace_back(*on_ground);
                                registry.replace<player_on_ground>(player_entity, next_entity);
                            }
                            return 0;
                        })
                        .or_else([&] -> std::optional<int> {
                            if (dir == seg.dir) {
                                // 进入地下模式。
                                turnings.emplace_back(*on_ground);
                                registry.emplace<player_under_ground>(
                                    player_entity,
                                    seg.start_center + direction_to_vector2f(seg.dir, seg.length)
                                        + direction_to_vector2f(dir, delta_length),
                                    dir);
                                registry.remove<player_on_ground>(player_entity);
                            }
                            return {};
                        });
                }
            } else {
                // 往前或往后。
                if (dir == seg.dir) {
                    registry.patch<segment>(seg_entity, [&](segment &seg) {
                        seg.walked_precent += delta_length / seg.length;
                        seg.wrap_walked_precent();
                    });
                } else if (negate_direction(dir) == seg.dir) {
                    registry.patch<segment>(seg_entity, [&](segment &seg) {
                        seg.walked_precent -= delta_length / seg.length;
                        seg.wrap_walked_precent();
                    });
                }
            }
        } else {
            // 地下模式。
            constexpr float epsilon = 5.f; //< 容错间隔。
            THR_ASSERT_MSG(under_ground != nullptr, "实体错误地同时没有 `thr::ecs::player_on_ground` 和 "
                                                    "`thr::ecs::player_under_ground` 组件。");
            sf::Vector2f             prev_position = under_ground->position;
            std::optional<direction> prev_dir = under_ground->prev_dir;
            sf::Vector2f next_position = prev_position + direction_to_vector2f(dir, delta_length);
            under_ground = &registry.replace<player_under_ground>(player_entity, next_position,
                                                                  dir); //< 避免指针失效。

            bool is_up_left_covered = false;
            bool is_up_right_covered = false;
            for (entt::entity current_entity :
                 scene_system::get_entities_in_same_scene(registry, player_entity)) {
                const auto *seg = registry.try_get<segment>(current_entity);
                if (seg == nullptr) {
                    continue;
                }
                if (no_nan_inf_f{seg->walked_precent} == no_nan_inf_f{0}) {
                    continue;
                }
                sf::FloatRect rect = [&] {
                    sf::FloatRect rect = seg->get_walked_bounds();
                    sf::Vector2f  start = rect.position;
                    sf::Vector2f  end = rect.position + rect.size;
                    auto [minx, maxx] = std::minmax(start.x, end.x);
                    auto [miny, maxy] = std::minmax(start.y, end.y);
                    return sf::FloatRect{{minx - epsilon, miny - epsilon},
                                         {maxx + (2 * epsilon) - minx, maxy + (2 * epsilon) - miny}};
                }();
                // 检测前面的两点。
                sf::Vector2f to_front =
                    direction_to_vector2f(dir, (player_under_ground::side_length / 2));
                sf::Vector2f to_left =
                    direction_to_vector2f(rotate_90_ccw(dir), (player_under_ground::side_length / 2));
                sf::Vector2f to_right =
                    direction_to_vector2f(rotate_90_cw(dir), (player_under_ground::side_length / 2));
                if (rect.contains(next_position + to_front + to_left)) {
                    is_up_left_covered = true;
                }
                if (rect.contains(next_position + to_front + to_right)) {
                    is_up_right_covered = true;
                }
                if (is_up_left_covered && is_up_right_covered) {
                    // 直接走。
                    if (prev_dir != dir) {
                        turnings.emplace_back(
                            player_under_ground{.position = prev_position, .prev_dir = prev_dir});
                    }
                    return;
                }
            }

            for (entt::entity current_entity :
                 scene_system::get_entities_in_same_scene(registry, player_entity)) {
                const auto *node = registry.try_get<struct node>(current_entity);
                // 尝试进入地表。
                if (node == nullptr) {
                    continue;
                }
                if (dir != registry.get<segment>(node->segment_entity).dir) {
                    continue;
                }
                auto rect = [&] -> sf::FloatRect {
                    sf::Vector2f position = node->position;
                    sf::Vector2f size;
                    switch (dir) {
                        case direction::right:
                            position += {epsilon, (node::side_length / 2) + epsilon};
                            size -=
                                {node::side_length + (2 * epsilon), node::side_length + (2 * epsilon)};
                            break;
                        case direction::left:
                            position -= {epsilon, (node::side_length / 2) + epsilon};
                            size +=
                                {node::side_length + (2 * epsilon), node::side_length + (2 * epsilon)};
                            break;
                        case direction::down:
                            position += {(node::side_length / 2) + epsilon, epsilon};
                            size -=
                                {node::side_length + (2 * epsilon), node::side_length + (2 * epsilon)};
                            break;
                        case direction::up:
                            position -= {(node::side_length / 2) + epsilon, epsilon};
                            size +=
                                {node::side_length + (2 * epsilon), node::side_length + (2 * epsilon)};
                            break;
                    }
                    return {position, size};
                }();
                if (rect.contains(next_position)) {
                    turnings.emplace_back(
                        player_under_ground{.position = prev_position, .prev_dir = prev_dir});
                    registry.remove<player_under_ground>(player_entity);
                    registry.emplace<player_on_ground>(player_entity, node->segment_entity);
                    return;
                }
            }
            registry.replace<player_under_ground>(player_entity, prev_position, prev_dir);
        }
    }

    void player_movement_system::undo(entt::registry &registry, entt::entity player_entity) noexcept {
        auto &turnings = registry.get<turning_history>(player_entity).turnings;
        if (turnings.empty()) {
            return;
        }
        if (const auto *on_ground = registry.try_get<player_on_ground>(player_entity)) {
            registry.patch<segment>(on_ground->segment_entity,
                                    [](segment &seg) { seg.walked_precent = 0.f; });
        }
        registry.remove<player_on_ground>(player_entity);
        registry.remove<player_under_ground>(player_entity);
        auto prev_player_status = turnings.back();
        turnings.pop_back();
        std::visit(
            make_overloaded(
                [&](player_on_ground on_ground) {
                    registry.emplace<player_on_ground>(player_entity, on_ground);
                    THR_ASSERT_MSG(
                        no_nan_inf_f{registry.get<segment>(on_ground.segment_entity).walked_precent}
                            == no_nan_inf_f{1},
                        "前一次的路径错误地没有走完。");
                },
                [&](player_under_ground under_ground) {
                    registry.emplace<player_under_ground>(player_entity, under_ground);
                }),
            prev_player_status);
    }

} // namespace thr::ecs