/**
 * @file player_movement_system.cpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 定义了玩家移动系统。
 * @version 0.1.0-5
 * @date 2026-08-01
 * 
 * @copyright cpp-love
 * 
 */

#include "thr/ecs/systems/player_movement_system.hpp"
#include "thr/base/assert_msg.hpp"
#include "thr/base/floating_point_compare.hpp"
#include "thr/base/overload.hpp"
#include "thr/ecs/components/global/game_base.hpp"
#include "thr/ecs/components/level_components.hpp"
#include "thr/ecs/components/maze_components.hpp"
#include "thr/ecs/components/player_components.hpp"
#include "thr/ecs/systems/global/scene_system.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <entt/entt.hpp>
#include <numbers>
#include <optional>
#include <spdlog/spdlog.h>
#include <utility>
#include <variant>

namespace thr::ecs {

    void player_movement_system::try_move(entt::registry &registry, entt::entity player_entity,
                                          float delta_length, combined_direction cdir,
                                          version_update_controller &controller) {
        if (cdir == combined_direction::none || no_nan_inf_f{delta_length} == no_nan_inf_f{0}) {
            // 不用动。
            return;
        }

        if (is_diagonal(cdir)) {
            // 让 `delta_length` 表示单一方向的移动长度。
            delta_length /= std::numbers::sqrt2_v<float>;
        }

        const auto &cur_player = registry.get<player>(player_entity);

        std::visit(
            make_overloaded(
                [&](const player::on_ground &on_ground) {
                    // 地上模式。
                    entt::entity seg_entity = on_ground.segment_entity;
                    const auto  &seg = registry.get<segment>(seg_entity);

                    if (no_nan_inf_f{seg.infos.get_current_state().walked_precent} != no_nan_inf_f{1}) {
                        // 没有走到底，所以尝试往前。
                        if (!has_direction(cdir, seg.dir)) {
                            // 无法移动，结束。
                            return;
                        }

                        registry.patch<segment>(seg_entity, [&](segment &seg) {
                            auto &info = controller.store_version(seg_entity, seg.infos);
                            info.walked_precent += delta_length / seg.length;
                            info.wrap_walked_precent();
                        });

                        cdir |= direction_to_combined_direction(negate_direction(seg.dir));

                        if (no_nan_inf_f{seg.infos.get_current_state().walked_precent} != no_nan_inf_f{1}
                            || cdir == combined_direction::none) {
                            // 还是没有走到底，或者方向为空，结束。
                            return;
                        }
                    }

                    // 走到底了，继续。
                    if (seg.next.has_value()) {
                        // 尝试进入下一段路。
                        const auto &next_seg = registry.get<segment>(*seg.next);

                        if (!has_direction(cdir, next_seg.dir)
                            || next_seg.infos.get_current_state().walked_precent
                                   != 0 /*有别的玩家在上面走*/) {
                            // 不行。
                            return;
                        }

                        registry.patch<segment>(seg_entity, [&](segment &cur_seg) {
                            auto &info = controller.store_version(seg_entity, cur_seg.infos);
                            info = {.prev_completed_entity = player_entity,
                                    .current_walking_entity{},
                                    .walked_precent = 0.f};
                        });
                        registry.patch<segment>(*seg.next, [&](segment &next_seg) {
                            auto &info = controller.store_version(*seg.next, next_seg.infos);
                            info.current_walking_entity = player_entity;
                            info.walked_precent = delta_length / seg.length;
                            info.wrap_walked_precent();
                        });
                        registry.patch<player>(player_entity, [&](player &player) {
                            auto &status = controller.store_version(player_entity, player.statuses);
                            status = player::on_ground{.segment_entity = *seg.next};
                        });
                        // 都返回了，不需要再修改了。
                        // cdir |= direction_to_combined_direction(negate_direction(next_seg.dir));
                        return;
                    }

                    // 尝试进入地下模式。
                    if (seg_entity == registry.ctx().get<level_info>().end_segment_entity) {
                        // 防止未结束时误进入地下模式。
                        return;
                    }

                    if (!has_direction(cdir, seg.dir)) {
                        // 不行。
                        return;
                    }

                    registry.patch<player>(player_entity, [&](player &player) {
                        auto &status = controller.store_version(player_entity, player.statuses);
                        status = player::under_ground{
                            .position = seg.get_end_center()
                                        + combined_direction_to_vector2f(cdir, delta_length),
                            .prev_dir = cdir};
                    });
                    // 都返回了，不需要再修改了。
                    // cdir |= direction_to_combined_direction(negate_direction(next_seg.dir));
                },
                [&](const player::under_ground &under_ground) {
                    combined_direction ori_cdir = cdir;
                    // 地下模式。
                    if (delta_length > player::side_length()) {
                        // 对于这种情况，下面函数的处理可能会出问题，但目前不想实现，所以警告一下。
                        /// @todo 完整此情况的处理。
                        spdlog::warn("当前移动的距离（大小：{}）大于玩家的边长，移动处理可能出问题。",
                                     delta_length);
                    }

                    const float half_length = player::side_length() / 2.f; //< 边长的一半。
                    const std::array<sf::Vector2f, 4> vertexs{{
                        sf::Vector2f{half_length, -half_length},  //< 右上角。
                        sf::Vector2f{half_length, half_length},   //< 右下角。
                        sf::Vector2f{-half_length, half_length},  //< 左下角。
                        sf::Vector2f{-half_length, -half_length}, //< 左上角。
                    }};

                    // 尝试直接走（看当前位置是否都覆盖）。
                    auto                              check_and_move = [&](combined_direction cdir) {
                        const sf::Vector2f next_position =
                            under_ground.position + combined_direction_to_vector2f(cdir, delta_length);
                        // 使用四个顶点代替整个检测。
                        std::array<bool, 4> is_position_covered{
                            false, //< 右上角。
                            false, //< 右下角。
                            false, //< 左下角。
                            false, //< 左上角。
                        };
                        for (entt::entity current_entity :
                             scene_system::get_entities_in_same_scene(registry, player_entity)) {
                            const auto *seg = registry.try_get<segment>(current_entity);
                            if (seg == nullptr) {
                                // 实体不拥有段，跳过。
                                continue;
                            }

                            const auto &info = seg->infos.get_current_state();
                            if (!info.prev_completed_entity.has_value() && info.walked_precent == 0) {
                                // 段没走过，跳过。
                                continue;
                            }

                            auto rect = [&] -> sf::FloatRect {
                                sf::FloatRect rect = info.prev_completed_entity.has_value()
                                                         ? seg->get_bounds()
                                                         : seg->get_walked_bounds();
                                sf::Vector2f  start = rect.position;
                                sf::Vector2f  end = rect.position + rect.size;
                                auto [minx, maxx] = std::minmax(start.x, end.x);
                                auto [miny, maxy] = std::minmax(start.y, end.y);
                                return {{minx - move_epsilon, miny - move_epsilon},
                                        {maxx + (2 * move_epsilon) - minx,
                                         maxy + (2 * move_epsilon) - miny}};
                            }();

                            for (auto &&[is_covered, pos] :
                                 std::views::zip(is_position_covered, vertexs)) {
                                if (!is_covered && rect.contains(pos + next_position)) {
                                    is_covered = true;
                                }
                            }
                        }

                        auto move = [&] {
                            if (under_ground.prev_dir != ori_cdir) {
                                if (!under_ground.position_last_recorded.has_value()
                                    || (*under_ground.position_last_recorded - under_ground.position)
                                               .length()
                                           > player::side_length()) {
                                    registry.patch<player>(player_entity, [&](player &player) {
                                        auto &status =
                                            controller.store_version(player_entity, player.statuses);
                                        status = player::under_ground{.position = next_position,
                                                                      .prev_dir = ori_cdir,
                                                                      .position_last_recorded =
                                                                          under_ground.position};
                                    });
                                }
                            } else {
                                registry.patch<player>(player_entity, [&](player &player) {
                                    auto &status =
                                        controller.store_version(player_entity, player.statuses);
                                    status = player::under_ground{.position = next_position,
                                                                  .prev_dir = ori_cdir};
                                });
                            }
                        };

                        if (is_orthogonal(cdir)) {
                            // 单方向。
                            auto idx = std::to_underlying(*combined_direction_to_direction(cdir));
                            THR_ASSERT_MSG(0 <= idx && idx <= 4, "转换后的索引不在范围内。");
                            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
                            if (is_position_covered[idx] && is_position_covered[(idx + 1) % 4]) {
                                // 直接走。
                                move();
                                return true;
                            }
                            return false;
                        }

                        // 多方向。
                        if (std::ranges::all_of(is_position_covered, [](bool value) { return value; })) {
                            // 直接走。
                            move();
                            return true;
                        }
                        return false;
                    };

                    if (check_and_move(cdir)) {
                        // 成功了。
                        return;
                    }

                    if (is_diagonal(cdir)) {
                        // 尝试分方向。
                        combined_direction vertical = get_vertical_component(cdir);
                        combined_direction horizontal = get_horizontal_component(cdir);
                        if (check_and_move(vertical)) {
                            cdir = horizontal;
                        } else if (check_and_move(horizontal)) {
                            cdir = vertical;
                        }
                    }

                    std::vector<sf::Vector2f> next_positions{
                        under_ground.position + combined_direction_to_vector2f(cdir, delta_length)};

                    if (is_diagonal(cdir)) {
                        // 记录分方向。
                        combined_direction vertical = get_vertical_component(cdir);
                        combined_direction horizontal = get_horizontal_component(cdir);
                        next_positions.reserve(3);
                        next_positions.emplace_back(
                            under_ground.position
                            + combined_direction_to_vector2f(vertical, delta_length));
                        next_positions.emplace_back(
                            under_ground.position
                            + combined_direction_to_vector2f(horizontal, delta_length));
                    }

                    // 尝试进入地表。
                    for (entt::entity current_entity :
                         scene_system::get_entities_in_same_scene(registry, player_entity)) {
                        const auto *node = registry.try_get<struct node>(current_entity);
                        if (node == nullptr) {
                            // 实体不拥有节点，跳过。
                            continue;
                        }

                        const auto &next_seg = registry.get<segment>(node->segment_entity);
                        if (!has_direction(cdir, next_seg.dir)
                            || next_seg.infos.get_current_state().walked_precent != 0.f) {
                            // 下一个段不可用，跳过。
                            continue;
                        }

                        sf::Vector2f  size = {node::side_length() + (2 * move_epsilon),
                                              node::side_length() + (2 * move_epsilon)};
                        sf::FloatRect rect{node->position - size / 2.f, size};
                        auto          segdir = std::to_underlying(next_seg.dir);
                        for (const sf::Vector2f &next_position : next_positions) {
                            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
                            if (rect.contains(next_position + vertexs[segdir])
                                // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
                                && rect.contains(next_position + vertexs[(segdir + 1) % 4])) {
                                // 可以进入地表。
                                registry.patch<player>(player_entity, [&](player &player) {
                                    auto &status =
                                        controller.store_version(player_entity, player.statuses);
                                    status = player::on_ground{.segment_entity = node->segment_entity};
                                });
                                registry.patch<segment>(node->segment_entity, [&](segment &seg) {
                                    auto &info =
                                        controller.store_version(node->segment_entity, seg.infos);
                                    info.current_walking_entity = player_entity;
                                });
                                return;
                            }
                        }
                    }
                }),
            cur_player.statuses.get_current_state());
    }

} // namespace thr::ecs