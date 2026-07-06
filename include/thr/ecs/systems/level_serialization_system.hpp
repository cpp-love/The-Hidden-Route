/**
 * @file level_serialization_system.hpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 声明了序列化关卡的系统。
 * @version 0.1.0-1
 * @date 2026-05-04
 * 
 * @copyright cpp-love
 * 
 */

#ifndef THR_ECS_SYSTEMS_LEVEL_SERIALIZATION_SYSTEM_HPP
#define THR_ECS_SYSTEMS_LEVEL_SERIALIZATION_SYSTEM_HPP

#include "thr/ecs/components/maze_components.hpp"
#include <entt/fwd.hpp>
#include <nlohmann/json_fwd.hpp>

namespace thr::ecs {

    /**
     * @brief 序列化关卡的系统。
     * @details 序列化的 JSON 格式可以见 [level.schema.json](../../../../schema/level-v0.1.0-3.schema.json)
     */
    class level_serialization_system {
      public:
        /**
         * @brief 将关卡序列化为 JSON。
         * @param [in] registry 注册表，里面有关卡组件。
         * @return nlohmann::json 序列化后的 JSON。
         */
        static nlohmann::json serialize_to_json(const entt::registry &registry);
        /**
         * @brief 将 JSON 反序列化为关卡。
         * @param [out] registry 注册表，用于添加关卡组件。
         * @param [in] json JSON。
         */
        static void deserialize_from_json(entt::registry &registry, const nlohmann::json &json);
    };

} // namespace thr::ecs

#endif // THR_ECS_SYSTEMS_LEVEL_SERIALIZATION_SYSTEM_HPP