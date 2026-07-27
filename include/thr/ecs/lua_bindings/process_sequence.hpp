/**
 * @file process_sequence.hpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 定义了实体的进程序列。
 * @version 0.1.0-1
 * @date 2026-07-24
 * 
 * @copyright cpp-love
 * 
 */

#ifndef THR_ECS_LUA_BINDINGS_SHEDULER_HPP
#define THR_ECS_LUA_BINDINGS_SHEDULER_HPP

#include "thr/ecs/components/global/game_base.hpp"
#include "thr/ecs/systems/animation_system.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Text.hpp>
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>
#include <entt/process/process.hpp>
#include <entt/process/scheduler.hpp>
#include <functional>
#include <optional>
#include <utility>

namespace thr::ecs::lua_bindings {

    /// @brief 实体的进程序列。
    class process_sequence {
      private:
        entt::handle                                   m_handle;       ///< 实体句柄。
        scheduler                                     &m_scheduler;    ///< 进程安排器。
        std::optional<std::reference_wrapper<process>> m_last_process; ///< 最后一个进程。

      public:
        /**
         * @brief 构造实体进程序列。
         * @param [in] handle 实体句柄。
         * @param [in] scheduler 进程安排器。
         */
        explicit process_sequence(entt::handle handle, scheduler &scheduler)
            : m_handle(handle), m_scheduler(scheduler) {}

        /**
         * @brief 添加文本淡入进程。
         * @param [in] duration_ms 以毫秒为单位的时间。
         * @return process_sequence& 对象本身，用于链式调用。
         */
        process_sequence &text_fade_in(float duration_ms) {
            return add_process<fade_in_process>(
                [handle = m_handle](std::uint8_t alpha) {
                    if (!handle.valid()) {
                        return;
                    }
                    auto *text = handle.try_get<sf::Text>();
                    if (text == nullptr) {
                        return;
                    }
                    sf::Color color = text->getFillColor();
                    color.a = alpha;
                    text->setFillColor(color);
                },
                milliseconds_f{duration_ms});
        }

        /**
         * @brief 添加文本淡出进程。
         * @param [in] duration_ms 以毫秒为单位的时间。
         * @return process_sequence& 对象本身，用于链式调用。
         */
        process_sequence &text_fade_out(float duration_ms) {
            return add_process<fade_out_process>(
                [handle = m_handle](std::uint8_t alpha) {
                    if (!handle.valid()) {
                        return;
                    }
                    auto *text = handle.try_get<sf::Text>();
                    if (text == nullptr) {
                        return;
                    }
                    sf::Color color = text->getFillColor();
                    color.a = alpha;
                    text->setFillColor(color);
                },
                milliseconds_f{duration_ms});
        }

        /**
         * @brief 添加等待进程。
         * @param [in] duration_ms 以毫秒为单位的时间。
         * @return process_sequence& 对象本身，用于链式调用。
         */
        process_sequence &wait(float duration_ms) {
            return add_process<wait_process>(milliseconds_f{duration_ms});
        }

        /**
         * @brief 添加销毁实体进程。
         * @return process_sequence& 对象本身，用于链式调用。
         */
        process_sequence &destroy_entity() { return add_process<destroy_entity_process>(m_handle); }

      private:
        /**
         * @brief 添加进程。
         * @tparam Type 进程类型。
         * @tparam Args 构造进程的参数类型。
         * @param [in] args 构造进程的参数。
         * @return process_sequence& 对象本身，用于链式调用。
         */
        template <typename Type, typename... Args>
        process_sequence &add_process(Args &&...args) {
            m_last_process =
                m_last_process
                    .transform([&](process &process) {
                        return std::reference_wrapper{process.then<Type>(std::forward<Args>(args)...)};
                    })
                    .or_else([&] {
                        return std::optional{std::reference_wrapper{
                            m_scheduler.attach<Type>(std::forward<Args>(args)...)}};
                    });
            return *this;
        }
    };

} // namespace thr::ecs::lua_bindings

#endif // THR_ECS_LUA_BINDINGS_SHEDULER_HPP