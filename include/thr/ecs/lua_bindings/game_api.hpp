/**
 * @file game_api.hpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 定义 `thr::ecs::lua_bindings::game_api` 类，集中声明为 Lua 提供的游戏 API。
 * @version 0.1.0-1
 * @date 2026-07-08
 * 
 * @copyright cpp-love
 * 
 */

#ifndef THR_ECS_LUA_BINDINGS_GAME_API_HPP
#define THR_ECS_LUA_BINDINGS_GAME_API_HPP

#include "thr/ecs/components/level_components.hpp"
#include "thr/ecs/configs.hpp"
#include "thr/ecs/lua_bindings/entity_wrapper.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/String.hpp>
#include <SFML/System/Vector2.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>
#include <functional>
#include <vector>

namespace thr::ecs::lua_bindings {

    /// @brief 集中声明为 Lua 提供的游戏 API 的类。
    class game_api {
      private:
        std::reference_wrapper<entt::registry> m_registry; ///< 注册表。

      public:
        /**
         * @brief 构建一个 game api 对象。
         * @param [in] registry 注册表。
         */
        explicit game_api(std::reference_wrapper<entt::registry> registry) noexcept
            : m_registry(registry) {}

        /**
         * @brief 创建一个实体（包装器）。
         * @return entity_wrapper 新建的实体包装器。
         */
        [[nodiscard]] entity_wrapper create_entity() { return {m_registry.get().create(), m_registry}; }

        /**
         * @brief 查询带标签的实体。
         * @param [in] tag_id 标签编号。
         * @return std::vector<entity_wrapper> 带标签的实体列表。
         */
        [[nodiscard]] std::vector<entity_wrapper> query_entities_with_tag(int tag_id) const {
            auto                        view = m_registry.get().view<tag>();
            std::vector<entity_wrapper> vector;
            for (const auto &[entity, tag] : view.each()) {
                if (tag.tag_ids.contains(tag_id)) {
                    vector.emplace_back(entity, m_registry);
                }
            }
            return vector;
        }

        /**
         * @brief 创建实体并为其添加文本。
         * @param [in] text 文本内容。
         * @param [in] position 位置。
         * @param [in] character_size 字号。
         * @return entity_wrapper 新建的实体（包装器）。
         */
        entity_wrapper add_text(std::string_view text, sf::Vector2f position,
                                unsigned int character_size = 30) {
            entt::entity entity = m_registry.get().create();

            sf::Text     sf_text{configs::singleton().get_sfml_font(),
                                 sf::String::fromUtf8(text.begin(), text.end()), character_size};
            sf_text.setPosition(position);
            sf_text.setLineAlignment(sf::Text::LineAlignment::Center);
            m_registry.get().emplace<sf::Text>(entity, sf_text);

            return {entity, m_registry};
        }
    };

} // namespace thr::ecs::lua_bindings

#endif // THR_ECS_LUA_BINDINGS_GAME_API_HPP