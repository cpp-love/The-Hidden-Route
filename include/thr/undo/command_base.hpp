/**
 * @file command_base.hpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 定义了操作的基类。
 * @version 0.1.0-1
 * @date 2026-08-17
 * 
 * @copyright cpp-love
 * 
 */

#ifndef THR_UNDO_COMMAND_BASE_HPP
#define THR_UNDO_COMMAND_BASE_HPP

#include "thr/ecs/components/global/game_base.hpp"

namespace thr::undo {

    /// @brief 操作基类。
    class command_base {
      public:
        /// @brief 构造 command base 对象。
        command_base() = default;
        /**
         * @brief 构造 command base 对象。
         * @param [in] rhs 另一个对象。
         */
        command_base(const command_base &rhs) noexcept = default;
        /**
         * @brief 构造 command base 对象。
         * @param [in] rhs 另一个对象。
         */
        command_base(command_base &&rhs) noexcept = default;
        /**
         * @brief 重载复制赋值运算符。
         * @param [in] rhs 另一个对象。
         * @return command_base& 对象本身。
         */
        command_base &operator=(const command_base &rhs) noexcept = default;
        /**
         * @brief 重载移动赋值运算符。
         * @param [in] rhs 另一个对象。
         * @return command_base& 对象本身。
         */
        command_base &operator=(command_base &&rhs) noexcept = default;
        /// @brief 重载虚析构函数。
        constexpr virtual ~command_base() = default;

        /**
         * @brief 更新操作。
         * @param [in] delta_time 时间间隔。
         */
        constexpr virtual void update(thr::ecs::milliseconds_f delta_time) = 0;
        /// @brief 撤销操作。
        constexpr virtual void undo() = 0;
        /// @brief 重做操作。
        constexpr virtual void redo() = 0;
    };

} // namespace thr::undo

#endif // THR_UNDO_COMMAND_BASE_HPP