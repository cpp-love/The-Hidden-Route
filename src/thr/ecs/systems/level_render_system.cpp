/**
 * @file level_render_system.cpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 定义了关卡渲染系统。
 * @version 0.1.0-4
 * @date 2026-07-14
 * 
 * @copyright cpp-love
 * 
 */

#include "thr/ecs/systems/level_render_system.hpp"
#include "thr/base/overload.hpp"
#include "thr/ecs/components/global/game_base.hpp"
#include "thr/ecs/components/maze_components.hpp"
#include "thr/ecs/components/player_components.hpp"
#include "thr/ecs/configs.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include <entt/entity/registry.hpp>
#include <utility>

namespace thr::ecs {

    void level_render_system::draw(const entt::registry &registry, sf::RenderTarget &render,
                                   const sf::RenderStates &states) {
        using namespace sf::Literals;

        // draw segments
        auto              list = registry.view<segment>();
        sf::RenderTexture render_texture{render.getSize()};
        render_texture.clear(configs::singleton().background_color);
        for (const auto &[entity, seg] : list.each()) {
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
        const auto        &players = registry.view<player>();
        const sf::Vector2f player_size{player::side_length(), player::side_length()};
        for (const auto &[entity, player] : players.each()) {
            std::visit(make_overloaded(
                           [&](const player::on_ground &on_ground) {
                               sf::RectangleShape rect_shape{player_size};
                               const auto        &seg = registry.get<segment>(on_ground.segment_entity);
                               rect_shape.setPosition(seg.get_current_center() - player_size / 2.f);
                               rect_shape.setFillColor(player.color);
                               render.draw(rect_shape, states);
                           },
                           [&](const player::under_ground &under_ground) {
                               sf::RectangleShape rect_shape{player_size};
                               rect_shape.setPosition(under_ground.position - player_size / 2.f);
                               sf::Color color = player.color;
                               color.a = configs::singleton().player_under_ground_render_alpha;
                               rect_shape.setFillColor(color);
                               render.draw(rect_shape, states);
                           }),
                       player.status);
        }

        // draw texts
        auto texts = registry.view<sf::Text>();
        for (const auto &[entity, text] : texts.each()) {
            render.draw(text, states);
        }

        // draw sprites
        auto sprites = registry.view<sf::Sprite>();
        for (const auto &[entity, sprite] : sprites.each()) {
            render.draw(sprite, states);
        }
    }

} // namespace thr::ecs