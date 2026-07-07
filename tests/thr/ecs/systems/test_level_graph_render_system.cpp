/**
 * @file test_level_graph_render_system.cpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief `thr::ecs::level_graph_render_system` 的简单回归测试。
 * @version 0.1.0-2
 * @date 2026-07-07
 * 
 * @copyright cpp-love
 * 
 */

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#undef NDEBUG
#include "thr/base/assert_msg.hpp"
#include "thr/ecs/components/level_graph_components.hpp"
#include "thr/ecs/systems/level_graph_render_system.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <entt/entity/registry.hpp>

int main() {
    sf::RenderTexture  render;
    tgui::Gui          gui(render);
    entt::registry     registry;

    const entt::entity start_entity = registry.create();
    const entt::entity next_entity = registry.create();
    registry.emplace<thr::ecs::level_node>(start_entity, std::vector<entt::entity>{next_entity},
                                           sf::Vector2f{100.f, 100.f}, "start", false);
    registry.emplace<thr::ecs::level_node>(next_entity, std::vector<entt::entity>{},
                                           sf::Vector2f{220.f, 160.f}, "next", true);

    thr::ecs::level_graph_render_system::draw(registry, gui, start_entity);

    auto start_button = gui.get<tgui::Button>("level_graph_node_0");
    THR_ASSERT_MSG(start_button != nullptr);
    THR_ASSERT_MSG(start_button->getText() == "start");
    auto next_button = gui.get<tgui::Button>("level_graph_node_1");
    THR_ASSERT_MSG(next_button != nullptr);
    THR_ASSERT_MSG(next_button->getText() == "next [locked]");

    return 0;
}
