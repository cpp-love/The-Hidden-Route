/**
 * @file level_graph_components.hpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 定义了一些关卡图的组件。
 * @version 0.1.0-1
 * @date 2026-07-04
 * 
 * @copyright cpp-love
 * 
 * @details 
 * - 采用图的方式存储关卡。
 */

#ifndef THR_ECS_COMPONENTS_LEVEL_GRAPH_COMPONENTS_HPP
#define THR_ECS_COMPONENTS_LEVEL_GRAPH_COMPONENTS_HPP

#include <SFML/System/Vector2.hpp>
#include <entt/entity/entity.hpp>
#include <vector>

namespace thr::ecs {

    /// @brief 关卡图的节点。
    struct level_node {
        std::vector<entt::entity> relative_entity; ///< 邻接的节点所属的实体。
        sf::Vector2f              position;        ///< 节点在关卡图中的位置。
        bool                      locked{};        ///< 节点是否锁住。
    };

} // namespace thr::ecs

#endif // THR_ECS_COMPONENTS_LEVEL_GRAPH_COMPONENTS_HPP