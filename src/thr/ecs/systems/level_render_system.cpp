/**
 * @file level_render_system.cpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 定义了迷宫渲染系统。
 * @version 0.1.0-2
 * @date 2026-06-19
 * 
 * @copyright cpp-love
 * 
 */

#include "thr/ecs/systems/level_render_system.hpp"
#include "thr/ecs/components/global/game_base.hpp"
#include "thr/ecs/components/maze_components.hpp"
#include "thr/ecs/components/player_components.hpp"
#include "thr/ecs/configs.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include <entt/entity/registry.hpp>
#include <utility>

namespace thr::ecs {

    void level_render_system::draw(const entt::registry &registry, sf::RenderTarget &render,
                                   const sf::RenderStates &states) noexcept {
        using namespace sf::Literals;
        // draw segments
        auto              list = registry.view<segment>();
        sf::RenderTexture render_texture{render.getSize()};
        render_texture.clear(configs::singleton().background_color);
        for (entt::entity entity : list) {
            const auto &seg = registry.get<segment>(entity);
            if (seg.walked_precent != 0.f) {
                sf::FloatRect      bound = seg.get_walked_bounds();
                sf::RectangleShape rect{bound.size};
                rect.setPosition(bound.position);
                rect.setFillColor(segment::color());
                render_texture.draw(rect);
            }
        }
        render_texture.display();
        sf::Sprite sprite{render_texture.getTexture()};
        sprite.setColor(configs::singleton().segments_render_opacity);
        render.draw(sprite, states);

        // draw lines
        const auto &lines = registry.ctx().get<line_strips>();
        render.draw(lines, states);

        // draw players
        const auto &player_on_grounds = registry.view<player_on_ground>();
        for (entt::entity player : player_on_grounds) {
            const auto        &on_ground = registry.get<player_on_ground>(player);
            const auto        &seg = registry.get<segment>(on_ground.segment_entity);
            sf::RectangleShape rect_shape{
                {player_on_ground::side_length(), player_on_ground::side_length()}};
            rect_shape.setPosition(seg.start_center
                                   + direction_to_vector2f(seg.dir, seg.length * seg.walked_precent)
                                   - sf::Vector2f{player_on_ground::side_length() / 2,
                                                  player_on_ground::side_length() / 2});
            rect_shape.setFillColor(player_on_ground::color());
            render.draw(rect_shape, states);
        }
        const auto &player_under_grounds = registry.view<player_under_ground>();
        for (entt::entity player : player_under_grounds) {
            const auto        &under_ground = registry.get<player_under_ground>(player);
            sf::RectangleShape rect_shape{
                {player_under_ground::side_length(), player_under_ground::side_length()}};
            rect_shape.setPosition(under_ground.position
                                   - sf::Vector2f{player_under_ground::side_length() / 2,
                                                  player_under_ground::side_length() / 2});
            rect_shape.setFillColor(player_under_ground::color());
            render.draw(rect_shape, states);
        }
    }

} // namespace thr::ecs