/**
 * @file maze_render_system.cpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 定义了迷宫渲染系统。
 * @version 0.1.0-1
 * @date 2026-05-02
 * 
 * @copyright cpp-love
 * 
 */

#include "thr/ecs/systems/maze_render_system.hpp"
#include "thr/ecs/components/maze_line_components.hpp"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include <entt/entity/registry.hpp>
#include <utility>

namespace thr::ecs {

    void maze_render_system::draw(const entt::registry &registry, sf::RenderTarget &render,
                                  const sf::RenderStates &states) noexcept {
        using namespace sf::Literals;
        // draw lines
        const auto &lines = registry.ctx().get<line_strips>();
        render.draw(lines, states);

        // draw segments
        auto list = registry.view<segment>();
        for (entt::entity entity : list) {
            const auto &seg = registry.get<segment>(entity);
            if (seg.walked_precent != 0.f) {
                sf::RectangleShape rect{{seg.length * seg.walked_precent, segment::width}};
                // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
                sf::Angle          angle = std::to_underlying(seg.dir) * 90_deg;
                rect.setPosition(seg.start_center
                                 + sf::Vector2f{0, -segment::width / 2}.rotatedBy(angle));
                rect.rotate(angle);
                rect.setFillColor(segment::color);
                render.draw(rect, states);
            }
        }
    }

} // namespace thr::ecs