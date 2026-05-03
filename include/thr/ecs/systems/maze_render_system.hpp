/**
 * @file maze_render_system.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 声明了迷宫渲染系统。
 * @version 0.1.0-1
 * @date 2026-05-02
 * 
 * @copyright cpp-love
 * 
 */

#ifndef TNRW_ECS_SYSTEMS_MAZE_RENDER_SYSTEM_HPP
#define TNRW_ECS_SYSTEMS_MAZE_RENDER_SYSTEM_HPP

#include "thr/ecs/components/maze_line_components.hpp"
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <entt/fwd.hpp>

namespace thr::ecs {

    class maze_render_system {
      public:
        /**
         * @brief 绘制迷宫。
         * @param [in] registry 注册表。
         * @param [in] render 渲染目标。
         * @param [in] states 渲染状态。
         */
        static void draw(const entt::registry &registry, sf::RenderTarget &render,
                         const sf::RenderStates &states = sf::RenderStates::Default) noexcept;
    };

}; // namespace thr::ecs

#endif // TNRW_ECS_SYSTEMS_MAZE_RENDER_SYSTEM_HPP