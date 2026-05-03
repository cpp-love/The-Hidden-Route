/**
 * @file maze_line_components.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 定义了迷宫线条组件。
 * @version 0.1.0-1
 * @date 2026-05-02
 * 
 * @copyright cpp-love
 * 
 */

#ifndef TNRW_ECS_COMPONENTS_MAZE_LINE_COMPONENTS_HPP
#define TNRW_ECS_COMPONENTS_MAZE_LINE_COMPONENTS_HPP

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <entt/entity/entity.hpp>
#include <vector>

namespace thr::ecs {

    /// @brief 方向枚举。
    enum class direction : std::uint8_t { right = 0, down, left, up };

    /// @brief 路径中直的一段。
    struct segment {
        constexpr static float     width = 50.f;              ///< 路径宽度
        constexpr static sf::Color color = sf::Color::Yellow; ///< 路径颜色

        entt::entity               prev{};                ///< 连接的前一个实体。
        entt::entity               next{};                ///< 连接的后一个实体。
        sf::Vector2f               start_center{};        ///< 路径启示位置中心
        float                      length{};              ///< 路径长度
        float                      walked_precent = 0.f;  ///< 走过的百分比。(范围：0~1)
        direction                  dir = direction::left; ///< 从前一个到后一个的方向。
    };

    /// @brief 路径中的“死胡同”。
    struct node {
        entt::entity prev{}; ///< 连接的前一个实体。
    };

    /// @brief 多段连续的折线。
    class line_strips : public sf::Drawable {
      private:
        std::vector<std::vector<sf::Vector2f>> m_vertexs;                  ///< 连续的折线。
        float                                  m_width = 5.f;              ///< 线段宽度。
        sf::Color                              m_color = sf::Color::White; ///< 线段颜色。

      public:
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
            : m_vertexs(std::move(vertexs)), m_width(width), m_color(color) {}

        /**
         * @brief 构建一个 line_strips 对象。
         * @param [in] vertexs 连续的折线。
         * @param [in] color 线段颜色。
         */
        explicit line_strips(std::vector<std::vector<sf::Vector2f>> vertexs,
                             sf::Color                              color = sf::Color::White)
            : m_vertexs(std::move(vertexs)), m_color(color) {}

        /**
         * @brief 获取连续的折线。
         * @return std::vector<std::vector<sf::Vector2f>>& 连续的折线。
         */
        [[nodiscard]] std::vector<std::vector<sf::Vector2f>> &get_vertexs() noexcept {
            return m_vertexs;
        }
        /**
         * @brief 获取连续的折线。
         * @return const std::vector<std::vector<sf::Vector2f>>& 连续的折线。
         */
        [[nodiscard]] const std::vector<std::vector<sf::Vector2f>> &get_vertexs() const noexcept {
            return m_vertexs;
        }

      protected:
        /**
         * @brief 绘制折线。
         * @param [in] target 渲染目标。
         * @param [in] states 渲染状态。
         */
        void draw(sf::RenderTarget &target, sf::RenderStates states) const override {
            for (const auto &strips : m_vertexs) {
                for (std::size_t i = 1; i < strips.size(); ++i) {
                    sf::Vector2f       line = strips[i] - strips[i - 1];
                    sf::Angle          angle = line.angle();
                    sf::RectangleShape rect{{line.length(), m_width}};
                    rect.setPosition(strips[i - 1] + sf::Vector2f{0, -m_width / 2}.rotatedBy(angle));
                    rect.rotate(angle);
                    rect.setFillColor(m_color);
                    target.draw(rect, states);
                }
            }
        }
    };

} // namespace thr::ecs

#endif // TNRW_ECS_COMPONENTS_MAZE_LINE_COMPONENTS_HPP