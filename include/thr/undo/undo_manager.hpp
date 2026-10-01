/**
 * @file undo_manager.hpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 定义了撤销-恢复管理器。
 * @version 0.1.0-1
 * @date 2026-08-23
 * 
 * @copyright cpp-love
 * 
 */

#ifndef THR_UNDO_UNDO_MANAGER_HPP
#define THR_UNDO_UNDO_MANAGER_HPP

#include "thr/base/assert_msg.hpp"
#include "thr/ecs/components/global/game_base.hpp"
#include "thr/undo/command_base.hpp"
#include <memory>
#include <vector>

namespace thr::undo {

    /// @brief 撤销-恢复管理器。
    class undo_manager {
      private:
        std::vector<std::unique_ptr<command_base>> m_history;    ///< 所有命令的历史。
        std::size_t                                m_position{}; ///< 当前命令位置的后一个位置。
      public:
        ///  @brief 构造 undo manager 对象。
        constexpr undo_manager() = default;
        /**
         * @brief 构造 undo manager 对象。
         * @param [in] history 历史版本。
         */
        constexpr explicit undo_manager(std::vector<std::unique_ptr<command_base>> history) noexcept
            : m_history(std::move(history)), m_position(m_history.size()) {}
        /**
         * @brief 构造 undo manager 对象。
         * @param [in] history 历史命令。
         * @param [in] position 当前命令位置的后一个位置。
         * @warning 请确保 `position` 参数在 `0 ~ history.size()` 的范围内，否则行为未定义。
         */
        constexpr explicit undo_manager(std::vector<std::unique_ptr<command_base>> history,
                                        std::size_t                                position) noexcept
            : m_history(std::move(history)), m_position(position) {
            THR_ASSERT_MSG(m_position <= m_history.size(),
                           "`position` 参数错误地不在 `0 ~ history.size()` 的范围内。");
        }

        /**
         * @brief 添加命令到栈中，并清空当前命令后面的所有历史。
         * @param [in] command 添加的命令。
         */
        constexpr void push(std::unique_ptr<command_base> command) {
            THR_ASSERT_MSG(command != nullptr, "传入的命令错误地为空。");
            clear_history_behind();
            m_history.push_back(std::move(command));
            m_position = m_history.size();
        }

        /**
         * @brief 更新当前命令。
         * @param [in] delta_time 时间间隔。
         * @note 此函数会自动调用 `command.update()`，无需用户手动调用。
         */
        constexpr void update(thr::ecs::milliseconds_f delta_time) {
            [[unlikely]] if (m_position == 0) { return; }
            clear_history_behind();
            m_history.back()->update(delta_time);
        }

        /**
         * @brief 撤销上一个操作。
         * @return true 撤销成功。
         * @return false 撤销失败：没有上一个操作。
         * @note 此函数会自动调用 `command.undo()`，无需用户手动调用。
         */
        constexpr bool undo() {
            if (m_position == 0) {
                return false;
            }
            --m_position;
            m_history[m_position]->undo();
            return true;
        }

        /**
         * @brief 恢复下一个操作。
         * @return true 恢复成功。
         * @return false 恢复失败：没有下一个操作。
         * @note 此函数会自动调用 `command.redo()`，无需用户手动调用。
         */
        constexpr bool redo() {
            if (m_position == m_history.size()) {
                return false;
            }
            m_history[m_position]->redo();
            ++m_position;
            return true;
        }

        /**
         * @brief 获取当前命令的索引。
         * @return std::size_t 当前命令的索引。
         */
        [[nodiscard]] constexpr std::size_t get_current_command_index() const noexcept {
            return m_position;
        }

      private:
        /// @brief 清空后面的历史。
        constexpr void clear_history_behind() {
            m_history.erase(
                m_history.begin()
                    + static_cast<std::vector<std::unique_ptr<command_base>>::difference_type>(
                        m_position),
                m_history.end());
        }
    };

} // namespace thr::undo

#endif // THR_UNDO_UNDO_MANAGER_HPP