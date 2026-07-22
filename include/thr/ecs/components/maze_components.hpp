/**
 * @file maze_components.hpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 定义了迷宫组件。
 * @version 0.1.0-4
 * @date 2026-07-22
 * 
 * @copyright cpp-love
 * 
 */

#ifndef THR_ECS_COMPONENTS_MAZE_COMPONENTS_HPP
#define THR_ECS_COMPONENTS_MAZE_COMPONENTS_HPP

#include "thr/ecs/components/global/game_base.hpp"
#include "thr/ecs/components/global/scene_components.hpp"
#include "thr/ecs/configs.hpp"
#include "thr/ecs/systems/global/scene_system.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <entt/entity/registry.hpp>
#include <optional>
#include <utility>
#include <vector>

namespace thr::ecs {

    /// @brief 路径中直的一段。
    struct segment {
        /**
         * @brief 从配置中获取路径宽度。
         * @return float 路径宽度。
         */
        static float                width() { return configs::singleton().segment_width; }
        /**
         * @brief 从配置中获取路径颜色。
         * @return sf::Color 路径颜色。
         */
        static sf::Color            color() { return configs::singleton().segment_color; }

        std::optional<entt::entity> prev{};                ///< （起始位置）连接的前一个路径实体。
        std::optional<entt::entity> next{};                ///< （终止位置）连接的后一个路径实体。
        sf::Vector2f                start_center;          ///< 路径起始位置中心。
        float                       length = 0.f;          ///< 路径长度。
        float                       walked_precent = 0.f;  ///< 被走过的百分比。(范围：0~1)
        direction                   dir = direction::left; ///< 从起始位置到终止位置的方向。

        /**
         * @brief 获取路径终止位置中心。
         * @return sf::Vector2f 路径终止位置中心。
         */
        [[nodiscard]] sf::Vector2f  get_end_center() const noexcept {
            return start_center + direction_to_vector2f(dir, length);
        }

        /**
         * @brief 获取包围范围。
         * @return sf::FloatRect 包围范围。
         */
        [[nodiscard]] sf::FloatRect get_bounds() const noexcept {
            sf::Vector2f position = start_center;
            sf::Vector2f size;
            switch (dir) {
                case direction::right:
                    position -= {segment::width() / 2, segment::width() / 2};
                    size += {length + segment::width(), segment::width()};
                    break;
                case direction::left:
                    position += {segment::width() / 2, segment::width() / 2};
                    size -= {length + segment::width(), segment::width()};
                    break;
                case direction::down:
                    position -= {segment::width() / 2, segment::width() / 2};
                    size += {segment::width(), length + segment::width()};
                    break;
                case direction::up:
                    position += {segment::width() / 2, segment::width() / 2};
                    size -= {segment::width(), length + segment::width()};
                    break;
            }
            return {position, size};
        }

        /**
         * @brief 获取行走过的包围范围。
         * @return sf::FloatRect 行走过的包围范围。
         */
        [[nodiscard]] sf::FloatRect get_walked_bounds() const noexcept {
            sf::Vector2f position = start_center;
            sf::Vector2f size;
            switch (dir) {
                case direction::right:
                    position -= {segment::width() / 2, segment::width() / 2};
                    size += {(length * walked_precent) + segment::width(), segment::width()};
                    break;
                case direction::left:
                    position += {segment::width() / 2, segment::width() / 2};
                    size -= {(length * walked_precent) + segment::width(), segment::width()};
                    break;
                case direction::down:
                    position -= {segment::width() / 2, segment::width() / 2};
                    size += {segment::width(), (length * walked_precent) + segment::width()};
                    break;
                case direction::up:
                    position += {segment::width() / 2, segment::width() / 2};
                    size -= {segment::width(), (length * walked_precent) + segment::width()};
                    break;
            }
            return {position, size};
        }

        /**
         * @brief 将 `walked_precent` 参数包装在 [0, 1] 区间内。
         * @details 若其大于 1，则将其设为 1；若其小于 0，则将其设为 0。
         */
        void wrap_walked_precent() noexcept {
            walked_precent = std::max(0.f, std::min(1.f, walked_precent));
        }

        /**
         * @brief 在构造 @ref segment 时调用的函数，用于设置其所属场景。
         * @param [in] registry 注册表。
         * @param [in] entity 构造了 @ref segment 的实体。
         */
        static void on_construct(entt::registry &registry, entt::entity entity) {
            const auto   &seg = registry.get<segment>(entity);
            sf::FloatRect bounds = seg.get_bounds();
            sf::Vector2f  start = bounds.position;
            sf::Vector2f  end = bounds.position + bounds.size;
            auto [minx, maxx] = std::minmax(start.x, end.x);
            auto [miny, maxy] = std::minmax(start.y, end.y);
            int col_start = static_cast<int>(minx / block_side_length);
            int col_end = static_cast<int>(maxx / block_side_length);
            int row_start = static_cast<int>(miny / block_side_length);
            int row_end = static_cast<int>(maxy / block_side_length);
            for (int row = row_start; row <= row_end; ++row) {
                for (int col = col_start; col <= col_end; ++col) {
                    auto scene_id = make_scene_identifier(row, col);
                    scene_system::insert_to_scene(registry, scene_id, entity);
                }
            }
        }

        /**
         * @brief 在更新 @ref segment 时调用的函数，用于设置其所属场景。
         * @param [in] registry 注册表。
         * @param [in] entity 更新了 @ref segment 的实体。
         */
        static void on_update(entt::registry &registry, entt::entity entity) {
            // 移除之前的场景登记，然后重新按当前矩形覆盖块插入
            scene_system::erase_from_all_scenes(registry, entity);
            on_construct(registry, entity);
        }

        /**
         * @brief 在移除 @ref segment 时调用的函数，用于设置其所属场景。
         * @param [in] registry 注册表。
         * @param [in] entity 移除了 @ref segment 的实体。
         */
        static void on_destroy(entt::registry &registry, entt::entity entity) {
            scene_system::erase_from_all_scenes(registry, entity);
        }

        /**
         * @brief 添加监听器，用于自动调用 @ref on_construct, @ref on_update, @ref on_destroy 来设置其所属场景。
         * @param [in] registry 注册表。
         */
        static void connect_listener(entt::registry &registry) {
            registry.on_construct<segment>().connect<&on_construct>();
            registry.on_update<segment>().connect<&on_update>();
            registry.on_destroy<segment>().connect<&on_destroy>();
        }

        /**
         * @brief 移除监听器，用于取消自动调用 @ref on_construct, @ref on_update, @ref on_destroy 来设置其所属场景。
         * @param [in] registry 注册表。
         */
        static void disconnect_listener(entt::registry &registry) {
            registry.on_construct<segment>().disconnect<&on_construct>();
            registry.on_update<segment>().disconnect<&on_update>();
            registry.on_destroy<segment>().disconnect<&on_destroy>();
        }
    };

    /// @brief 路径中的“死胡同”。
    struct node {
        constexpr static float side_length = 20.f;         ///< “死胡同”的边长。
        sf::Vector2f           position;                   ///< 中心位置。
        entt::entity           segment_entity{entt::null}; ///< 指向的节点位置。

        /**
         * @brief 在构造 @ref node 时调用的函数，用于设置其所属场景。
         * @param [in] registry 注册表。
         * @param [in] entity 构造了 @ref node 的实体。
         */
        static void            on_construct(entt::registry &registry, entt::entity entity) {
            const auto  &node = registry.get<struct node>(entity);
            sf::Vector2f start =
                node.position - sf::Vector2f{node::side_length / 2, node::side_length / 2};
            sf::Vector2f end =
                node.position + sf::Vector2f{node::side_length / 2, node::side_length / 2};
            int col_start = static_cast<int>(start.x / block_side_length);
            int col_end = static_cast<int>(end.x / block_side_length);
            int row_start = static_cast<int>(start.y / block_side_length);
            int row_end = static_cast<int>(end.y / block_side_length);
            for (int row = row_start; row <= row_end; ++row) {
                for (int col = col_start; col <= col_end; ++col) {
                    auto scene_id = make_scene_identifier(row, col);
                    scene_system::insert_to_scene(registry, scene_id, entity);
                }
            }
        }

        /**
         * @brief 在更新 @ref node 时调用的函数，用于设置其所属场景。
         * @param [in] registry 注册表。
         * @param [in] entity 更新了 @ref node 的实体。
         */
        static void on_update(entt::registry &registry, entt::entity entity) {
            // 移除之前的场景登记，然后重新按当前矩形覆盖块插入
            scene_system::erase_from_all_scenes(registry, entity);
            on_construct(registry, entity);
        }

        /**
         * @brief 在移除 @ref node 时调用的函数，用于设置其所属场景。
         * @param [in] registry 注册表。
         * @param [in] entity 移除了 @ref node 的实体。
         */
        static void on_destroy(entt::registry &registry, entt::entity entity) {
            scene_system::erase_from_all_scenes(registry, entity);
        }

        /**
         * @brief 添加监听器，用于自动调用 @ref on_construct, @ref on_update, @ref on_destroy 来设置其所属场景。
         * @param [in] registry 注册表。
         */
        static void connect_listener(entt::registry &registry) {
            registry.on_construct<node>().connect<&on_construct>();
            registry.on_update<node>().connect<&on_update>();
            registry.on_destroy<node>().connect<&on_destroy>();
        }

        /**
         * @brief 移除监听器，用于取消自动调用 @ref on_construct, @ref on_update, @ref on_destroy 来设置其所属场景。
         * @param [in] registry 注册表。
         */
        static void disconnect_listener(entt::registry &registry) {
            registry.on_construct<node>().disconnect<&on_construct>();
            registry.on_update<node>().disconnect<&on_update>();
            registry.on_destroy<node>().disconnect<&on_destroy>();
        }
    };

    /// @brief 多段连续的折线。
    class line_strips : public sf::Drawable {
      public:
        std::vector<std::vector<sf::Vector2f>> vertexs;                  ///< 连续的折线。
        static constexpr float                 default_width = 5.f;      ///< 默认线段宽度。
        float                                  width = default_width;    ///< 线段宽度。
        sf::Color                              color = sf::Color::White; ///< 线段颜色。

        /// @brief 构建一个 line_strips 对象。
        line_strips() = default;
        /**
         * @brief 构建一个 line_strips 对象。
         * @param [in] vertexs 连续的折线。
         * @param [in] width 线段宽度。
         * @param [in] color 线段颜色。
         */
        explicit line_strips(std::vector<std::vector<sf::Vector2f>> vertexs, float width,
                             sf::Color color = sf::Color::White)
            : vertexs(std::move(vertexs)), width(width), color(color) {}
        /**
         * @brief 构建一个 line_strips 对象。
         * @param [in] vertexs 连续的折线。
         * @param [in] color 线段颜色。
         */
        explicit line_strips(std::vector<std::vector<sf::Vector2f>> vertexs,
                             sf::Color                              color = sf::Color::White)
            : vertexs(std::move(vertexs)), color(color) {}

      protected:
        /**
         * @brief 绘制折线。
         * @param [in] target 渲染目标。
         * @param [in] states 渲染状态。
         */
        void draw(sf::RenderTarget &target, sf::RenderStates states) const override {
            for (const auto &strips : vertexs) {
                for (std::size_t i = 1; i < strips.size(); ++i) {
                    sf::Vector2f       line = strips[i] - strips[i - 1];
                    sf::Angle          angle = line.angle();
                    sf::RectangleShape rect{{line.length(), width}};
                    rect.setPosition(strips[i - 1] + sf::Vector2f{0, -width / 2}.rotatedBy(angle));
                    rect.rotate(angle);
                    rect.setFillColor(color);
                    target.draw(rect, states);
                }
                for (const sf::Vector2f &point : strips) {
                    sf::CircleShape circle{width / 2};
                    circle.setPosition(point - sf::Vector2f{width / 2, width / 2});
                    circle.setFillColor(color);
                    target.draw(circle, states);
                }
            }
        }
    };

} // namespace thr::ecs

#endif // THR_ECS_COMPONENTS_MAZE_COMPONENTS_HPP