/**
 * @file animation_system.hpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 定义了动画相关的系统。
 * @version 0.1.0-1
 * @date 2026-07-22
 * 
 * @copyright cpp-love
 * 
 */

#ifndef THR_ECS_SYSTEMS_ANIMATION_SYSTEM_HPP
#define THR_ECS_SYSTEMS_ANIMATION_SYSTEM_HPP

#include "thr/ecs/components/global/game_base.hpp"
#include <algorithm>
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>
#include <entt/process/process.hpp>
#include <functional>

namespace thr::ecs {

    /// @brief 淡入进程。
    class fade_in_process : public process {
      public:
        using callback_t = std::function<void(std::uint8_t)>;

      private:
        callback_t m_callback;    ///< 回调。
        delta_type m_duration{0}; ///< 持续时间。
        delta_type m_elapsed{0};  ///< 淡入的已经过时间。

      public:
        /**
         * @brief 构造淡入进程。
         * @param [in] allocator 分配器。
         * @param [in] callback 回调函数（参数为当前的 alpha 值，没有返回值）。
         * @param [in] duration 淡入的持续时间。
         */
        fade_in_process(const allocator_type &allocator, callback_t callback, delta_type duration)
            : entt::basic_process<milliseconds_f>{allocator}, m_callback(std::move(callback)),
              m_duration(duration) {}

      private:
        void update(delta_type delta, void * /*unused*/) override {
            m_elapsed += delta;
            float progress = std::min(1.f, m_elapsed.count() / m_duration.count());
            m_callback(static_cast<std::uint8_t>(progress * 255u));

            if (progress == 1.f) {
                succeed();
            }
        }
    };

    /// @brief 淡出进程。
    class fade_out_process : public process {
      public:
        using callback_t = std::function<void(std::uint8_t)>;

      private:
        callback_t m_callback;    ///< 回调。
        delta_type m_duration{0}; ///< 持续时间。
        delta_type m_elapsed{0};  ///< 淡出的已经过时间。

      public:
        /**
         * @brief 构造淡出进程。
         * @param [in] allocator 分配器。
         * @param [in] callback 回调函数（参数为当前的 alpha 值，没有返回值）。
         * @param [in] duration 淡出的持续时间。
         */
        fade_out_process(const allocator_type &allocator, callback_t callback, delta_type duration)
            : entt::basic_process<milliseconds_f>{allocator}, m_callback(std::move(callback)),
              m_duration(duration) {}

      private:
        void update(delta_type delta, void * /*unused*/) override {
            m_elapsed += delta;
            float progress = std::min(1.f, m_elapsed.count() / m_duration.count());
            m_callback(static_cast<std::uint8_t>((1.f - progress) * 255u));

            if (progress == 1.f) {
                succeed();
            }
        }
    };

    /// @brief 等待进程。
    class wait_process : public process {
      private:
        delta_type m_remaining{0}; ///< 剩余的等待时间。

      public:
        /**
         * @brief 构造等待进程。
         * @param [in] allocator 分配器。
         * @param [in] duration 等待时间。
         */
        wait_process(const allocator_type &allocator, delta_type duration)
            : entt::basic_process<milliseconds_f>{allocator}, m_remaining(duration) {}

      private:
        void update(delta_type delta, void * /*unused*/) override {
            m_remaining -= delta;

            if (m_remaining <= 0_ms_f) {
                succeed();
            }
        }
    };

    /// @brief 销毁实体进程。
    class destroy_entity_process : public process {
      private:
        entt::handle m_handle; ///< 实体句柄。

      public:
        /**
         * @brief 构造等待进程。
         * @param [in] allocator 分配器。
         * @param [in] handle 实体句柄。
         */
        destroy_entity_process(const allocator_type &allocator, entt::handle handle)
            : entt::basic_process<milliseconds_f>{allocator}, m_handle(handle) {}

      private:
        void update(delta_type /*unused*/, void * /*unused*/) override {
            if (!m_handle.valid()) {
                fail();
                return;
            }
            m_handle.destroy();
            succeed();
        }
    };

} // namespace thr::ecs

#endif // THR_ECS_SYSTEMS_ANIMATION_SYSTEM_HPP