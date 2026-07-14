/**
 * @file level_graph_render_system.hpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 定义了渲染关卡图的系统。
 * @version 0.1.0-4
 * @date 2026-07-12
 * 
 * @copyright cpp-love
 * 
 */

#include "thr/ecs/systems/level_graph_render_system.hpp"
#include "thr/base/assert_msg.hpp"
#include "thr/ecs/configs.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <TGUI/AbsoluteOrRelativeValue.hpp>
#include <TGUI/Container.hpp>
#include <TGUI/Layout.hpp>
#include <TGUI/String.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/Panel.hpp>
#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entt.hpp>
#include <set>
#include <string>
#include <string_view>
#include <utility>

namespace thr::ecs {
    namespace {
        constexpr float      button_scale = 0.075f;
        const tgui::Layout2d button_size{tgui::RelativeValue(button_scale),
                                         tgui::RelativeValue(button_scale)}; ///< 按钮大小。

        /*
        void                   remove_existing_nodes(tgui::Gui &gui) noexcept {
            auto widgets = gui.getWidgets();
            for (const auto &widget : widgets) {
                if (widget->getWidgetName().starts_with(
                        tgui::String(level_graph_render_system::widget_prefix))) {
                    gui.remove(widget);
                }
            }
        }
        */

        /**
         * @brief 绘制线段。
         * @param [in] render 渲染目标。
         * @param [in] start 线段起点。
         * @param [in] end 线段终点。
         * @param [in] color 线段颜色。
         */
        void draw_edge(sf::RenderTarget &render, const sf::Vector2f &start, const sf::Vector2f &end,
                       const sf::Color &color = sf::Color::White) {
            sf::VertexArray line(sf::PrimitiveType::Lines, 2);
            line[0].position = start;
            line[1].position = end;
            line[0].color = color;
            line[1].color = color;
            render.draw(line);
        }
    } // namespace

    void level_graph_render_system::draw(const entt::registry &registry, tgui::Container::Ptr container,
                                         sf::RenderTarget &target, entt::entity entity,
                                         node_callback on_click) noexcept {
        THR_ASSERT_MSG(registry.valid(entity), "起始实体无效。");
        // 不每次销毁并重建所有按钮，复用已有 widgets，减少频繁创建影响 UI。

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

            tgui::Button::Ptr    button = container->get<tgui::Button>(widget_id_name);

            const tgui::Layout2d position{
                tgui::RelativeValue(node.position.x
                                    / static_cast<float>(configs::singleton().game_screen_size.x)),
                tgui::RelativeValue(node.position.y
                                    / static_cast<float>(configs::singleton().game_screen_size.y))};

            if (button == nullptr) {
                button = tgui::Button::create();
                button->getRenderer()->setRoundedBorderRadius(10.f);
                if (on_click) {
                    button->onPress([on_click, current] { on_click(current); });
                }
                container->add(button, widget_id_name);
            }
            // 复用已有按钮：只更新状态和位置，避免重新分配和重复连接回调。
            button->setPosition(position);
            button->setSize(button_size);
            button->setEnabled(!node.locked);
            button->setText(node.locked ? std::format("{} [locked]", node.name) : node.name);

            sf::Vector2f half_button_real_size = {
                static_cast<float>(configs::singleton().game_screen_size.x) * button_scale / 2,
                static_cast<float>(configs::singleton().game_screen_size.y) * button_scale / 2};

            // 向下遍历。
            for (entt::entity next : node.relative_entities) {
                THR_ASSERT_MSG(registry.valid(next), "下一个实体(id: {})不合法",
                               entt::to_integral(next));
                const auto &next_node = registry.get<level_node>(next);
                if (!node.locked) {
                    // 重复画没有关系，因为无效果。
                    draw_edge(target, node.position + half_button_real_size,
                              next_node.position + half_button_real_size, sf::Color::White);
                }
                self(next);
            }
        };

        visit(entity);
    }

} // namespace thr::ecs