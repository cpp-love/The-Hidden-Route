/**
 * @file maze_line_components.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 定义了迷宫线条组件。
 * @version 0.1.0-2
 * @date 2026-05-09
 * 
 * @copyright cpp-love
 * 
 */

#ifndef THR_ECS_COMPONENTS_MAZE_LINE_COMPONENTS_HPP
#define THR_ECS_COMPONENTS_MAZE_LINE_COMPONENTS_HPP

#include "thr/base/assert_msg.hpp"
#include "thr/ecs/components/global/scene_components.hpp"
#include "thr/ecs/systems/global/scene_system.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <cstdint>
#include <entt/entity/registry.hpp>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace thr::ecs {

    /// @brief 方向枚举。
    enum class direction : std::uint8_t { right = 0, down, left, up };

    /**
     * @brief 获取方向名字。
     * @param [in] dir 方向。
     * @return std::string 名字。
     */
    inline std::string direction_to_name(direction dir) {
        static_assert(std::to_underlying(direction::up) == 3, "此函数对于 direction 枚举的处理不够");
        switch (dir) {
            case direction::right: return "right";
            case direction::down: return "down";
            case direction::left: return "left";
            case direction::up: return "up";
        }
        unreachable("参数 dir（值为：{}）非法", std::to_underlying(dir));
    }

    /**
     * @brief 从方向名字获取方向枚举。
     * @param [in] dir 方向名字。
     * @return direction 方向枚举。
     * @exception std::invalid_argument 当方向名字非法时输出
     */
    inline direction name_to_direction(std::string_view dir) {
        static_assert(std::to_underlying(direction::up) == 3, "此函数对于 direction 枚举的处理不够");
        if (dir == "right") {
            return direction::right;
        }
        if (dir == "down") {
            return direction::down;
        }
        if (dir == "left") {
            return direction::left;
        }
        if (dir == "right") {
            return direction::up;
        }
        throw std::invalid_argument("in thr::ecs::from_direction_name(): unsupported direction name.");
    }

    /**
     * @brief 以指定长度转换到 `sf::Vector2f`。
     * @param [in] dir 方向。
     * @param [in] length 指定的长度。
     * @return sf::Vector2f 转换结果。
     */
    inline sf::Vector2f direction_to_vector2f(direction dir, float length) {
        static_assert(std::to_underlying(direction::up) == 3, "此函数对于 direction 枚举的处理不够");
        switch (dir) {
            case direction::right: return {length, 0};
            case direction::down: return {0, length};
            case direction::left: return {-length, 0};
            case direction::up: return {0, -length};
        }
        unreachable("参数 dir（值为：{}）非法", std::to_underlying(dir));
    }

    constexpr float block_side_length = 50.f; ///< 区域块的长度。

    /// @brief 路径中直的一段。
    struct segment {
        constexpr static float      width = 50.f;              ///< 路径宽度。
        constexpr static sf::Color  color = sf::Color::Yellow; ///< 路径颜色。

        std::optional<entt::entity> prev{};                ///< （起始位置）连接的前一个路径实体。
        std::optional<entt::entity> next{};                ///< （终止位置）连接的后一个路径实体。
        sf::Vector2f                start_center;          ///< 路径起始位置中心。
        float                       length = 0.f;          ///< 路径长度。
        float                       walked_precent = 0.f;  ///< 被走过的百分比。(范围：0~1)
        direction                   dir = direction::left; ///< 从起始位置到终止位置的方向。

        static void                 on_construct(entt::registry &registry, entt::entity entity) {
            const auto  &seg = registry.get<segment>(entity);
            sf::Vector2f start = seg.start_center;
            sf::Vector2f end = seg.start_center;
            switch (seg.dir) {
                case direction::right:
                    start -= {0.f, segment::width / 2};
                    end += {seg.length, segment::width / 2};
                    break;
                case direction::left:
                    start += {0.f, segment::width / 2};
                    end += {-seg.length, -segment::width / 2};
                    break;
                case direction::down:
                    start -= {segment::width / 2, 0.f};
                    end += {segment::width / 2, seg.length};
                    break;
                case direction::up:
                    start += {segment::width / 2, 0.f};
                    end += {-segment::width / 2, -seg.length};
                    break;
            }
            auto [minx, maxx] = std::minmax(start.x, end.x);
            auto [miny, maxy] = std::minmax(start.x, end.x);
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

        static void on_update(entt::registry &registry, entt::entity entity) {
            // 移除之前的场景登记，然后重新按当前矩形覆盖块插入
            scene_system::erase_from_all_scenes(registry, entity);
            on_construct(registry, entity);
        }

        static void on_destroy(entt::registry &registry, entt::entity entity) {
            scene_system::erase_from_all_scenes(registry, entity);
        }

        static void add_listener(entt::registry &registry) {
            registry.on_construct<segment>().connect<&on_construct>();
            registry.on_update<segment>().connect<&on_update>();
            registry.on_destroy<segment>().connect<&on_destroy>();
        }
    };

    /// @brief 路径中的“死胡同”。
    /// @warning 此组件所属的实体一定要有 @ref thr::ecs::segment 组件。
    struct node {
        sf::Vector2f position; ///< 位置。

        static void  on_construct(entt::registry &registry, entt::entity entity) {
            const auto &node = registry.get<struct node>(entity);
            int         col = static_cast<int>(node.position.x / block_side_length);
            int         row = static_cast<int>(node.position.y / block_side_length);
            auto        scene_id = make_scene_identifier(row, col);
            scene_system::insert_to_scene(registry, scene_id, entity);
        }

        static void on_update(entt::registry &registry, entt::entity entity) {
            // 移除之前的场景登记，然后重新按当前矩形覆盖块插入
            scene_system::erase_from_all_scenes(registry, entity);
            on_construct(registry, entity);
        }

        static void on_destroy(entt::registry &registry, entt::entity entity) {
            scene_system::erase_from_all_scenes(registry, entity);
        }

        static void add_listener(entt::registry &registry) {
            registry.on_construct<segment>().connect<&on_construct>();
            registry.on_update<segment>().connect<&on_update>();
            registry.on_destroy<segment>().connect<&on_destroy>();
        }
    };

    /// @brief 多段连续的折线。
    class line_strips : public sf::Drawable {
      public:
        std::vector<std::vector<sf::Vector2f>> vertexs;                  ///< 连续的折线。
        float                                  width = 5.f;              ///< 线段宽度。
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
            }
        }
    };

} // namespace thr::ecs

#endif // THR_ECS_COMPONENTS_MAZE_LINE_COMPONENTS_HPP