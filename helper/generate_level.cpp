/**
 * @file generate_level.cpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 关卡辅助生成器。
 * @version 0.1.0-1
 * @date 2026-07-15
 * 
 * @copyright cpp-love
 * 
 */

#include "thr/base/assert_msg.hpp"
#include "thr/base/floating_point_compare.hpp"
#include "thr/ecs/components/global/game_base.hpp"
#include "thr/ecs/components/global/game_state_components.hpp"
#include "thr/ecs/components/level_components.hpp"
#include "thr/ecs/components/maze_components.hpp"
#include "thr/ecs/components/player_components.hpp"
#include "thr/ecs/configs.hpp"
#include "thr/ecs/systems/global/game_state_manager.hpp"
#include "thr/ecs/systems/level_render_system.hpp"
#include "thr/ecs/systems/level_serialization_system.hpp"
#include "thr/ecs/systems/player_movement_system.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/String.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <array>
#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>
#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>
#include <sol/types.hpp>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

/// @brief 生成器状态类。
class generator_level : public thr::ecs::game_state_base {
  public:
    static constexpr sf::Vector2f start_position = {320.f, 240.f}; ///< 初始起始位置。

    /// @copydoc game_state_base::game_state_base()
    generator_level() noexcept;
    /// @copydoc game_state_base::game_state_base(const game_state_base &rhs)
    generator_level(const generator_level &rhs) noexcept = delete;
    /// @copydoc game_state_base::game_state_base(game_state_base &&rhs)
    generator_level(generator_level &&rhs) noexcept = default;
    /// @copydoc game_state_base::operator=(const game_state_base &rhs)
    generator_level &operator=(const generator_level &rhs) noexcept = delete;
    /// @copydoc game_state_base::operator=(game_state_base &&rhs)
    generator_level &operator=(generator_level &&rhs) noexcept = delete;
    /// @copydoc game_state_base::~game_state_base
    ~generator_level() noexcept override;
    /// @copydoc game_state_base::on_pause
    void on_pause() noexcept override {}
    /// @copydoc game_state_base::on_resume
    void on_resume() noexcept override {}
    /// @copydoc game_state_base::on_handle_event
    bool handle_event(const sf::Event &event) noexcept override;
    /// @copydoc game_state_base::update
    void update(thr::ecs::milliseconds_f delta_time) noexcept override;
    /// @copydoc game_state_base::draw
    void draw() noexcept override;

  protected:
    /// @copydoc game_state_base::init
    void init() noexcept override;

  private:
    /**
     * @brief 将当前角色的移动转换为一个 `segment` 道路段，并维护前后连接关系及可视化边界。
     * @param [in] dir 方向。
     * @param [in] position 位置。
     */
    void                      move(thr::ecs::direction dir, sf::Vector2f position) noexcept;
    /// @brief 为注册表添加线段。
    void                      add_line_strips() noexcept;
    // /// @brief 撤销最后一个绘制点，同时删除对应的 `segment` 实体。
    // void                        remove_last_point() noexcept;
    /// @brief 保存当前关卡数据到 JSON 文件。
    void                      save_level() noexcept;
    /// @brief 重置生成器状态并重新创建玩家实体。
    void                      reset_level() noexcept;

    entt::entity              m_player_entity{entt::null}; ///< 玩家实体。
    entt::registry            m_registry;                  ///< 注册表。
    std::vector<entt::entity> m_segment_entities;          ///< 当前路径对应的 `segment` 实体序列。
    std::vector<sf::Vector2f> m_points;                    ///< 当前正在编辑的路径点。
    bool                      m_started_painting{false};   ///< 是否开始铺路。
};

generator_level::generator_level() noexcept {
    constexpr sf::Vector2f     hint_position = {12.f, 12.f}; //< 提示位置。
    constexpr unsigned int     hint_character_size = 16u;    //< 提示的字号大小。
    constexpr std::string_view hint_str{
        "WASD/方向键移动角色 | J 开始铺路 | K 撤销（尚未实现） | Enter 保存 | Delete "
        "重置"}; //< 提示字符串。

    m_registry.ctx().emplace<thr::ecs::line_strips>(std::vector<std::vector<sf::Vector2f>>{});
    m_registry.ctx().emplace<thr::ecs::level_info>();
    m_player_entity = m_registry.create();
    m_registry.emplace<thr::ecs::player_under_ground>(m_player_entity, start_position);
    m_registry.destroy(m_player_entity);
    m_player_entity = m_registry.create();
    m_registry.emplace<thr::ecs::player_under_ground>(m_player_entity, start_position);

    sf::Text hint{thr::ecs::configs::singleton().get_sfml_font(),
                  sf::String::fromUtf8(hint_str.begin(), hint_str.end()), hint_character_size};
    hint.setPosition(hint_position);
    hint.setFillColor(sf::Color::White);
    entt::entity hint_entity = m_registry.create();
    m_registry.emplace<sf::Text>(hint_entity, std::move(hint));
}

generator_level::~generator_level() noexcept = default;

void generator_level::init() noexcept {}

void generator_level::update(thr::ecs::milliseconds_f delta_time) noexcept {
    constexpr float velocity_per_millisecond = 0.2f; ///< 移动速度。
    if (m_window->hasFocus()) {
        const float                        move_length = velocity_per_millisecond * delta_time.count();
        std::optional<thr::ecs::direction> dir;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)
            || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
            dir = thr::ecs::direction::up;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)
                   || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
            dir = thr::ecs::direction::down;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)
                   || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
            dir = thr::ecs::direction::left;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)
                   || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
            dir = thr::ecs::direction::right;
        }

        if (dir.has_value()) {
            m_registry.patch<thr::ecs::player_under_ground>(
                m_player_entity, [&](thr::ecs::player_under_ground &under_ground) {
                    const sf::Vector2f prev_position = under_ground.position;
                    under_ground.position += thr::ecs::direction_to_vector2f(*dir, move_length);

                    under_ground.prev_dir =
                        under_ground.prev_dir
                            .transform([&](thr::ecs::direction prev_dir) {
                                if (*dir == thr::ecs::negate_direction(prev_dir)) {
                                    // 不能动。
                                    under_ground.position -=
                                        thr::ecs::direction_to_vector2f(*dir, move_length);
                                    return prev_dir;
                                }
                                if (*dir != prev_dir) {
                                    // 换方向了，需要添加点。
                                    THR_ASSERT_MSG(m_started_painting,
                                                   "`under_ground.prev_dir` 的内部处理错误。");
                                    move(prev_dir, prev_position);
                                }
                                return *dir;
                            })
                            .or_else([&] -> std::optional<thr::ecs::direction> {
                                if (m_started_painting) {
                                    // 刚开始画，添加初始点。
                                    THR_ASSERT_MSG(m_points.empty(), "`.m_points` 内部处理错误。");
                                    m_points.push_back(prev_position);
                                    const auto &lines = m_registry.ctx().get<thr::ecs::line_strips>();
                                    const auto &level_info =
                                        m_registry.ctx().get<thr::ecs::level_info>();
                                    THR_ASSERT_MSG(m_segment_entities.empty(),
                                                   "`.m_points` 内部处理错误。");
                                    THR_ASSERT_MSG(level_info.start_segment_entity == entt::null,
                                                   "`.m_points` 内部处理错误。");
                                    THR_ASSERT_MSG(level_info.end_segment_entity == entt::null,
                                                   "`.m_points` 内部处理错误。");
                                    THR_ASSERT_MSG(lines.vertexs.empty(), "`.m_points` 内部处理错误。");

                                    return dir;
                                }
                                return std::nullopt;
                            });
                });
        }
    }
}

void generator_level::move(thr::ecs::direction dir, sf::Vector2f position) noexcept {
    auto &level_info = m_registry.ctx().get<thr::ecs::level_info>();

    THR_ASSERT_MSG(!m_points.empty(), "`.m_points` 内部处理错误。");

    const sf::Vector2f prev_pos = m_points.back();
    const sf::Vector2f delta = position - prev_pos;
    const float        length = delta.length();

    m_points.push_back(position);

    // 添加 segment。
    // 模板段用于计算包围盒和切分点，但不直接作为实体存在。
    thr::ecs::segment   segment_template{.start_center = prev_pos,
                                         .length = length,
                                         .walked_precent = 1,
                                         .dir = dir};
    const sf::FloatRect bounds = segment_template.get_bounds();

    if (m_segment_entities.empty()) {
        // 是首段，直接创建单段。
        entt::entity entity = m_registry.create();
        level_info.start_segment_entity = entity;
        level_info.end_segment_entity = entity;
        m_registry.emplace<thr::ecs::segment>(entity, segment_template);
        m_segment_entities.push_back(entity);
        add_line_strips();
        return;
    }

    // 查询并收集所有与本段相交的交点（可能多次下穿）。
    std::vector<std::array<sf::Vector2f, 2>> intersections; //< 与其他段的相交区域的左右端点。
    for (const entt::entity current : m_segment_entities) {
        // 跳过紧邻的上一段，避免误判。
        const auto &cur_seg = m_registry.get<thr::ecs::segment>(current);
        if (cur_seg.get_end_center() == segment_template.start_center) {
            continue;
        }
        sf::FloatRect cur_bounds = cur_seg.get_bounds();

        if (auto intersection = bounds.findIntersection(cur_bounds)) {
            if ((thr::ecs::is_vertical(dir)
                 && thr::no_nan_inf_f{intersection->size.x,
                                      thr::ecs::player_movement_system::move_epsilon}
                        == thr::no_nan_inf_f{thr::ecs::segment::width(),
                                             thr::ecs::player_movement_system::move_epsilon})
                || (thr::ecs::is_horizontal(dir)
                    && thr::no_nan_inf_f{intersection->size.y,
                                         thr::ecs::player_movement_system::move_epsilon}
                           == thr::no_nan_inf_f{thr::ecs::segment::width(),
                                                thr::ecs::player_movement_system::move_epsilon})) {
                intersections.push_back(
                    {intersection->position, intersection->position + intersection->size});
            }
            // 其他情况仅为有重合部分，非下穿。
        }
    }

    if (intersections.empty()) {
        // 若没有交点则按普通方式连接。
        entt::entity       entity = m_registry.create();
        const entt::entity prev_entity = m_segment_entities.back();
        segment_template.prev = prev_entity;
        m_registry.patch<thr::ecs::segment>(prev_entity,
                                            [&](thr::ecs::segment &seg) { seg.next = entity; });
        level_info.end_segment_entity = entity;
        m_registry.emplace<thr::ecs::segment>(entity, segment_template);
        m_segment_entities.push_back(entity);
        add_line_strips();
        return;
    }

    // 函数作用：计算在当前段上的投影长度。
    auto proj = [&](const sf::Vector2f &pos) -> float {
        const sf::Vector2f vec = pos - prev_pos;
        return vec.dot(delta) / length;
    };

    // 函数作用：将投影长度转为在当前段上的点。
    auto convert_to_point = [&](float length) -> sf::Vector2f {
        return prev_pos + thr::ecs::direction_to_vector2f(dir, length);
    };

    // 按左端点排序。
    std::ranges::sort(intersections, [&](const std::array<sf::Vector2f, 2> &lhs,
                                         const std::array<sf::Vector2f, 2> &rhs) {
        return proj(lhs[0]) < proj(rhs[0]);
    });

    float        proj_prev_end = -thr::ecs::segment::width() / 2; //< 前一个结尾的投影长度。
    entt::entity last_created = entt::null;                       //< 前一个创建的实体。

    auto         connect_if_created_first = [&](entt::entity new_entity, thr::ecs::segment &new_seg) {
        if (last_created == entt::null) {
            const entt::entity prev_entity = m_segment_entities.back();
            const auto        &prev_seg = m_registry.get<thr::ecs::segment>(prev_entity);
            if (prev_seg.get_end_center() == new_seg.start_center) {
                // 第一个新创建的段，连接到已有链的末端。
                new_seg.prev = prev_entity;
                m_registry.patch<thr::ecs::segment>(
                    prev_entity, [&](thr::ecs::segment &seg) { seg.next = new_entity; });
            }
        }
    };

    for (const auto &[first, second] : intersections) {
        auto [proj_l, proj_r] = std::minmax({proj(first), proj(second)});

        if (thr::no_nan_inf_f{proj_prev_end, thr::ecs::player_movement_system::move_epsilon}
            >= thr::no_nan_inf_f{proj_l, thr::ecs::player_movement_system::move_epsilon}) {
            // 区间重复，不添加。
            // 更新结尾值。
            proj_prev_end = std::max(proj_r, proj_prev_end);
            continue;
        }

        // 创建 segment。
        const entt::entity new_entity = m_registry.create();
        thr::ecs::segment new_seg{.start_center =
                                      convert_to_point(proj_prev_end + (thr::ecs::segment::width() / 2)),
                                  .length = proj_l - proj_prev_end - thr::ecs::segment::width(),
                                  .walked_precent = 1,
                                  .dir = dir};

        connect_if_created_first(new_entity, new_seg);

        m_registry.emplace<thr::ecs::segment>(new_entity, new_seg);
        m_segment_entities.push_back(new_entity);
        last_created = new_entity;
        // 更新结尾值。
        proj_prev_end = proj_r;
    }

    if (thr::no_nan_inf_f{proj_prev_end, thr::ecs::player_movement_system::move_epsilon}
        < thr::no_nan_inf_f{length + (thr::ecs::segment::width() / 2),
                            thr::ecs::player_movement_system::move_epsilon}) {
        // 创建 segment。
        const entt::entity new_entity = m_registry.create();
        thr::ecs::segment new_seg{.start_center =
                                      convert_to_point(proj_prev_end + (thr::ecs::segment::width() / 2)),
                                  .length = length - proj_prev_end - (thr::ecs::segment::width() / 2),
                                  .walked_precent = 1,
                                  .dir = dir};

        connect_if_created_first(new_entity, new_seg);

        m_registry.emplace<thr::ecs::segment>(new_entity, new_seg);
        m_segment_entities.push_back(new_entity);
        last_created = new_entity;
    }

    if (last_created != entt::null) {
        level_info.end_segment_entity = last_created;
    }

    add_line_strips();
}

void generator_level::add_line_strips() noexcept {
    auto &lines = m_registry.ctx().get<thr::ecs::line_strips>();
    lines.vertexs.clear();

    if (m_segment_entities.empty()) {
        return;
    }

    // 转换为矩形。
    std::vector<sf::FloatRect> rects;
    rects.reserve(m_segment_entities.size());
    for (const entt::entity entity : m_segment_entities) {
        auto rect = m_registry.get<thr::ecs::segment>(entity).get_bounds();
        if (rect.size.x < 0) {
            rect.position.x += rect.size.x;
            rect.size.x = -rect.size.x;
        }
        if (rect.size.y < 0) {
            rect.position.y += rect.size.y;
            rect.size.y = -rect.size.y;
        }
        rects.push_back(rect);
    }

    for (size_t i = 0; i < m_segment_entities.size(); ++i) {
        const auto &rect_i = rects[i];
        const auto &seg_i = m_registry.get<thr::ecs::segment>(m_segment_entities[i]);
        // 矩形每一条边中要绘制的线集合（从小到大排序）。
        std::array<std::vector<float>, 4> lines_in_edges{
            {{rect_i.position.x, rect_i.position.x + rect_i.size.x} /*上横边*/,
             {rect_i.position.y, rect_i.position.y + rect_i.size.y} /*右竖边*/,
             {rect_i.position.x, rect_i.position.x + rect_i.size.x} /*下横边*/,
             {rect_i.position.y, rect_i.position.y + rect_i.size.y} /*左竖边*/}};

        // 函数作用：从线的集合中排除一段。
        auto extract_from_lines = [](std::vector<float> &lines, float low, float high) {
            auto it_low = std::ranges::lower_bound(lines, low);
            // 如果刚好在线段的右端点，自增迭代器。
            if (it_low != lines.end() && *it_low == low && (it_low - lines.begin()) % 2 == 1) {
                ++it_low;
            }
            auto it_high = std::ranges::lower_bound(lines, high);
            // 如果刚好在线段的右端点，自增迭代器。
            if (it_high != lines.end() && *it_high == high && (it_high - lines.begin()) % 2 == 1) {
                ++it_high;
            }

            // 重新裁减范围。
            auto pos_low = it_low - lines.begin();
            auto pos_high = it_high - lines.begin();

            bool need_low = (pos_low % 2 == 1);
            bool need_high = (pos_high % 2 == 1);

            if (need_low || need_high) {
                lines.reserve(lines.size() + 2); //< 避免重分配。

                // 先插入后面的，再插入前面的。
                if (need_high) {
                    lines.insert(lines.begin() + pos_high, high);
                }
                if (need_low) {
                    lines.insert(lines.begin() + pos_low, low);
                    ++pos_high;
                    ++pos_low;
                }
            }

            lines.erase(lines.begin() + pos_low, lines.begin() + pos_high);
        };

        for (size_t j = 0; j < i; ++j) {
            if (m_segment_entities[j] == seg_i.prev) {
                continue;
            }
            const auto &rect_j = rects[j];
            if (const auto intersection = rect_i.findIntersection(rect_j)) {
                if (thr::no_nan_inf_f{intersection->position.x}
                    == thr::no_nan_inf_f{rect_i.position.x}) {
                    // 左竖边重合。
                    extract_from_lines(lines_in_edges[3], intersection->position.y,
                                       intersection->position.y + intersection->size.y);
                }
                if (thr::no_nan_inf_f{intersection->position.y}
                    == thr::no_nan_inf_f{rect_i.position.y}) {
                    // 上横边重合。
                    extract_from_lines(lines_in_edges[0], intersection->position.x,
                                       intersection->position.x + intersection->size.x);
                }
                if (thr::no_nan_inf_f{intersection->position.x + intersection->size.x}
                    == thr::no_nan_inf_f{rect_i.position.x + rect_i.size.x}) {
                    // 右竖边重合。
                    extract_from_lines(lines_in_edges[1], intersection->position.y,
                                       intersection->position.y + intersection->size.y);
                }
                if (thr::no_nan_inf_f{intersection->position.y + intersection->size.y}
                    == thr::no_nan_inf_f{rect_i.position.y + rect_i.size.y}) {
                    // 下横边重合。
                    extract_from_lines(lines_in_edges[2], intersection->position.x,
                                       intersection->position.x + intersection->size.x);
                }
            }
        }

        auto extract = [&](sf::Vector2f pos) {
            if (thr::no_nan_inf_f{pos.x} == thr::no_nan_inf_f{rect_i.position.x}) {
                // 左竖边重合。
                extract_from_lines(lines_in_edges[3], pos.y - (thr::ecs::segment::width() / 2),
                                   pos.y + (thr::ecs::segment::width() / 2));
            }
            if (thr::no_nan_inf_f{pos.y} == thr::no_nan_inf_f{rect_i.position.y}) {
                // 上横边重合。
                extract_from_lines(lines_in_edges[0], pos.x - (thr::ecs::segment::width() / 2),
                                   pos.x + (thr::ecs::segment::width() / 2));
            }
            if (thr::no_nan_inf_f{pos.x} == thr::no_nan_inf_f{rect_i.position.x + rect_i.size.x}) {
                // 右竖边重合。
                extract_from_lines(lines_in_edges[1], pos.y - (thr::ecs::segment::width() / 2),
                                   pos.y + (thr::ecs::segment::width() / 2));
            }
            if (thr::no_nan_inf_f{pos.y} == thr::no_nan_inf_f{rect_i.position.y + rect_i.size.y}) {
                // 下横边重合。
                extract_from_lines(lines_in_edges[2], pos.x - (thr::ecs::segment::width() / 2),
                                   pos.x + (thr::ecs::segment::width() / 2));
            }
        };

        if (seg_i.prev.has_value()) {
            const auto  &seg_prev = m_registry.get<thr::ecs::segment>(*seg_i.prev);
            sf::Vector2f pos =
                seg_i.start_center
                + thr::ecs::direction_to_vector2f(thr::ecs::negate_direction(seg_prev.dir),
                                                  thr::ecs::segment::width() / 2);

            extract(pos);
        }
        if (seg_i.next.has_value()) {
            const auto  &seg_next = m_registry.get<thr::ecs::segment>(*seg_i.next);
            sf::Vector2f pos =
                seg_next.start_center
                + thr::ecs::direction_to_vector2f(seg_next.dir, thr::ecs::segment::width() / 2);

            extract(pos);
        }

        for (size_t j = 0; j + 1 < lines_in_edges[0].size(); j += 2) {
            lines.vertexs.push_back({{lines_in_edges[0][j], rect_i.position.y},
                                     {lines_in_edges[0][j + 1], rect_i.position.y}});
        }
        for (size_t j = 0; j + 1 < lines_in_edges[1].size(); j += 2) {
            lines.vertexs.push_back({{rect_i.position.x + rect_i.size.x, lines_in_edges[1][j]},
                                     {rect_i.position.x + rect_i.size.x, lines_in_edges[1][j + 1]}});
        }
        for (size_t j = 0; j + 1 < lines_in_edges[2].size(); j += 2) {
            lines.vertexs.push_back({{lines_in_edges[2][j], rect_i.position.y + rect_i.size.y},
                                     {lines_in_edges[2][j + 1], rect_i.position.y + rect_i.size.y}});
        }
        for (size_t j = 0; j + 1 < lines_in_edges[3].size(); j += 2) {
            lines.vertexs.push_back({{rect_i.position.x, lines_in_edges[3][j]},
                                     {rect_i.position.x, lines_in_edges[3][j + 1]}});
        }
    }
}

// void generator_level::remove_last_point() noexcept {
//     if (m_points.empty()) {
//         return;
//     }

//     m_points.pop_back();
//     auto &lines = m_registry.ctx().get<thr::ecs::line_strips>();
//     if (!lines.vertexs.empty()) {
//         lines.vertexs.back().pop_back();
//         if (lines.vertexs.back().empty()) {
//             lines.vertexs.pop_back();
//         }
//     }

//     if (m_segment_entities.empty()) {
//         return;
//     }

//     const entt::entity last_entity = m_segment_entities.back();
//     m_segment_entities.pop_back();
//     m_registry.destroy(last_entity);

//     auto &level_info = m_registry.ctx().get<thr::ecs::level_info>();
//     if (!m_segment_entities.empty()) {
//         const entt::entity prev_entity = m_segment_entities.back();
//         auto               prev_segment = m_registry.get<thr::ecs::segment>(prev_entity);
//         prev_segment.next.reset();
//         m_registry.replace<thr::ecs::segment>(prev_entity, prev_segment);
//         level_info.end_segment_entity = prev_entity;
//     } else {
//         level_info.start_segment_entity = entt::null;
//         level_info.end_segment_entity = entt::null;
//     }
// }

void generator_level::reset_level() noexcept {
    m_points.clear();
    m_started_painting = false;

    auto &lines = m_registry.ctx().get<thr::ecs::line_strips>();
    lines.vertexs.clear();

    for (const entt::entity entity : m_segment_entities) { m_registry.destroy(entity); }
    m_segment_entities.clear();

    auto &level_info = m_registry.ctx().get<thr::ecs::level_info>();
    level_info.start_segment_entity = entt::null;
    level_info.end_segment_entity = entt::null;

    THR_ASSERT_MSG(m_registry.valid(m_player_entity), "generator_level 内部逻辑有误。");
    m_registry.destroy(m_player_entity);
    m_player_entity = m_registry.create();
    m_registry.emplace<thr::ecs::player_under_ground>(
        m_player_entity,
        thr::ecs::player_under_ground{.position = start_position, .prev_dir = std::nullopt});
}

void generator_level::save_level() noexcept {
    const std::filesystem::path output_path = "assets/json/generated_level.json";
    std::filesystem::create_directories(output_path.parent_path());

    std::ofstream output(output_path);
    if (!output.is_open()) {
        spdlog::error("无法写入关卡文件: {}", output_path.string());
        return;
    }

    for (auto [entity, seg] : m_registry.view<thr::ecs::segment>().each()) { seg.walked_precent = 0; }
    const nlohmann::json json = thr::ecs::level_serialization_system::serialize_to_json(m_registry);
    for (auto [entity, seg] : m_registry.view<thr::ecs::segment>().each()) { seg.walked_precent = 1; }
    output << json.dump(4) << '\n';
    spdlog::info("已保存生成器关卡到 {}", output_path.string());
}

bool generator_level::handle_event(const sf::Event &event) noexcept {

    // if (const auto *mouse = event.getIf<sf::Event::MouseButtonReleased>()) {
    //     if (mouse->button == sf::Mouse::Button::Left) {
    //         m_started_painting = false;
    //         m_preview_point.reset();
    //         return true;
    //     }
    // }

    if (const auto *key = event.getIf<sf::Event::KeyPressed>()) {
        switch (key->code) {
            case sf::Keyboard::Key::Enter: save_level(); return true;
            case sf::Keyboard::Key::Delete: reset_level(); return true;
            case sf::Keyboard::Key::J:
                m_started_painting = true;
                return true;
                // case sf::Keyboard::Key::K:
                //     m_started_painting = false;
                //     remove_last_point();
                //     return true;
            default: break;
        }
    }

    return false;
}

void generator_level::draw() noexcept { thr::ecs::level_render_system::draw(m_registry, *m_window); }

int  main() {

#ifdef _WIN32
    // 让Windows支持UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif // _WIN32

    // spdlog config
    auto file_logger = std::make_shared<spdlog::sinks::basic_file_sink_st>("log/log.log");
    file_logger->set_level(spdlog::level::trace);
    auto stdout_logger = std::make_shared<spdlog::sinks::stdout_sink_st>();
    stdout_logger->set_level(spdlog::level::info);
    auto multi_sink_logger = std::make_shared<spdlog::logger>(
        "multi_sink_logger", spdlog::sinks_init_list{file_logger, stdout_logger});
    multi_sink_logger->set_level(spdlog::level::trace);
    spdlog::set_default_logger(multi_sink_logger);
    spdlog::flush_on(spdlog::level::info);

    sf::RenderWindow             window(sf::VideoMode(thr::ecs::configs::singleton().game_screen_size),
                                        "The Hidden Route - Level Generator");
    thr::ecs::game_state_manager manager(window);
    manager.push_state(std::make_unique<generator_level>());

    auto prev = thr::ecs::clock::now();
    while (true) {
        // handle event
        while (const std::optional event = window.pollEvent()) { manager.handle_event(*event); }
        if (!window.isOpen()) {
            break;
        }

        // update
        auto cur = thr::ecs::clock::now();
        manager.update(cur - prev);
        prev = cur;

        // render
        window.clear(thr::ecs::configs::singleton().background_color);
        manager.draw();
        window.display();
    }

    return 0;
}