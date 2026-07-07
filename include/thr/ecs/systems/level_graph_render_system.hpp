/**
 * @file level_graph_render_system.hpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 声明了渲染关卡图的系统。
 * @version 0.1.0-1
 * @date 2026-07-06
 * 
 * @copyright cpp-love
 * 
 */

#ifndef THR_ECS_SYSTEMS_LEVEL_GRAPH_RENDER_SYSTEM_HPP
#define THR_ECS_SYSTEMS_LEVEL_GRAPH_RENDER_SYSTEM_HPP

#include "thr/ecs/components/level_graph_components.hpp"
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <entt/fwd.hpp>
#include <functional>

namespace thr::ecs {

    /// @brief 渲染关卡图的系统。
    class level_graph_render_system {
      public:
        static constexpr std::string_view widget_prefix{"level_graph_node_"}; ///< 添加的 TGUI 按钮名称。
        using node_callback = std::function<void(entt::entity)>;

        /**
         * @brief 渲染关卡图并生成对应的 GUI 按钮。
         * @param [in] registry 注册表。
         * @param [in] gui GUI 对象。
         * @param [in] entity 从此实体开始遍历。
         * @param [in] on_click 点击节点时触发的回调。
         */
        static void draw(const entt::registry &registry, tgui::Gui &gui, entt::entity entity,
                         node_callback on_click = {}) noexcept;
    };

} // namespace thr::ecs

#endif // THR_ECS_SYSTEMS_LEVEL_GRAPH_RENDER_SYSTEM_HPP