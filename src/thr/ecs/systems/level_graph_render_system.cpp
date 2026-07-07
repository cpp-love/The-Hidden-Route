/**
 * @file level_graph_render_system.hpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 定义了渲染关卡图的系统。
 * @version 0.1.0-2
 * @date 2026-07-07
 * 
 * @copyright cpp-love
 * 
 */

#include "thr/ecs/systems/level_graph_render_system.hpp"
#include "thr/base/assert_msg.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <TGUI/Layout.hpp>
#include <TGUI/String.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entt.hpp>
#include <set>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace thr::ecs {
    namespace {
        constexpr sf::Vector2f button_size{80.f, 40.f};

        void                   remove_existing_nodes(tgui::Gui &gui) noexcept {
            auto widgets = gui.getWidgets();
            for (const auto &widget : widgets) {
                if (widget->getWidgetName().starts_with(
                        tgui::String(level_graph_render_system::widget_prefix))) {
                    gui.remove(widget);
                }
            }
        }

        void draw_edge(sf::RenderTarget &render, const sf::Vector2f &from, const sf::Vector2f &to,
                       const sf::Color &color = sf::Color::White) {
            sf::VertexArray line(sf::PrimitiveType::Lines, 2);
            line[0].position = from;
            line[1].position = to;
            line[0].color = color;
            line[1].color = color;
            render.draw(line);
        }
    } // namespace

    void level_graph_render_system::draw(const entt::registry &registry, tgui::Gui &gui,
                                         entt::entity entity, node_callback on_click) noexcept {
        THR_ASSERT_MSG(registry.valid(entity), "起始实体无效。");
        // 不每次销毁并重建所有按钮，复用已有 widgets，减少频繁创建影响 UI。
        sf::RenderTarget *render_target = gui.getTarget(); //< 渲染目标。
        THR_ASSERT_MSG(render_target != nullptr, "GUI 对象没有渲染目标。");

        std::set<entt::entity> visited; //< 是否访问过。

        auto                   visit = [&](this auto &self, entt::entity current) noexcept -> void {
            // 是否有效且未访问过。
            THR_ASSERT_MSG(registry.valid(current), "实体(id: {})不合法", entt::to_integral(current));
            if (!visited.insert(current).second) {
                return;
            }

            // 添加按钮。
            const auto       &node = registry.get<level_node>(current);
            // 使用实体 id 作为 widget 名称，保证跨帧稳定性，便于复用。
            const std::string widget_id_name =
                std::format("{}{}", widget_prefix, entt::to_integral(current));
            tgui::Button::Ptr  button = gui.get<tgui::Button>(widget_id_name);
            const sf::Vector2f pos = node.position;
            if (button) {
                // 复用已有按钮：只更新状态和位置，避免重新分配和重复连接回调。
                button->setText(node.locked ? std::format("{} [locked]", node.name) : node.name);
                button->setPosition({pos.x, pos.y});
                button->setSize(tgui::Layout2d{button_size});
                button->setEnabled(!node.locked);
            } else {
                button = tgui::Button::create(node.locked ? std::format("{} [locked]", node.name)
                                                          : node.name);
                button->setPosition({pos.x, pos.y});
                button->setSize(tgui::Layout2d{button_size});
                button->setEnabled(!node.locked);
                if (on_click) {
                    button->onPress([on_click, current] { on_click(current); });
                }
                gui.add(button, widget_id_name);
            }

            // 向下遍历。
            for (entt::entity next : node.relative_entities) {
                THR_ASSERT_MSG(registry.valid(next), "下一个实体(id: {})不合法",
                               entt::to_integral(next));
                const auto &next_node = registry.get<level_node>(next);
                if (!node.locked) {
                    // 重复画没有关系，因为无效果。
                    draw_edge(*render_target, node.position + button_size / 2.f,
                              next_node.position + button_size / 2.f, sf::Color::White);
                }
                self(next);
            }
        };

        visit(entity);
    }

} // namespace thr::ecs