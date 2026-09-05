/**
 * @file with_history.hpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 定义了可以记录历史的模板类。
 * @version 0.1.0-1
 * @date 2026-08-17
 * 
 * @copyright cpp-love
 * 
 */

#ifndef THR_BASE_WITH_HISTORY_HPP
#define THR_BASE_WITH_HISTORY_HPP

#include "thr/base/assert_msg.hpp"
#include <cstddef>
#include <initializer_list>
#include <utility>
#include <vector>

namespace thr {

    /**
     * @brief 可以记录历史的模板类。
     * @tparam T 存储类型。
     */
    template <typename T>
    class with_history {
      private:
        std::vector<T> m_history;  ///< 所有历史。
        std::size_t    m_position; ///< 当前状态所在位置。
      public:
        ///  @brief 构造 with history 对象。
        constexpr with_history() : m_history(1), m_position(0) {}
        /**
         * @brief 构造 with history 对象。
         * @param [in] history 历史版本。
         * @warning 请确保 `history` 参数非空，否则行为未定义。
         */
        constexpr explicit with_history(std::vector<T> history) noexcept
            : m_history(std::move(history)), m_position(m_history.size() - 1) {
            THR_ASSERT_MSG(!m_history.empty(), "`history` 参数错误地为空。");
        }
        /**
         * @brief 构造 with history 对象。
         * @param [in] history 历史版本。
         * @param [in] position 当前的位置。
         * @warning 请确保 `history` 参数非空，否则行为未定义。
         * @warning 请确保 `position` 参数在 `0 ~ history.size() - 1` 的范围内，否则行为未定义。
         */
        constexpr explicit with_history(std::vector<T> history, std::size_t position) noexcept
            : m_history(std::move(history)), m_position(position) {
            THR_ASSERT_MSG(!m_history.empty(), "`history` 参数错误地为空。");
            THR_ASSERT_MSG(m_position < m_history.size(),
                           "`position` 参数错误地不在 `0 ~ history.size() - 1` 的范围内。");
        }

        /// @brief 清空所有历史。
        constexpr void clear() {
            m_history.clear();
            m_history.emplace_back();
            m_position = 0;
        }

        /**
         * @brief 保存当前状态的一份副本，在后面追加新的状态，并清空当前状态后面的所有历史。
         * @param [in] new_state 新的状态（默认为默认构造）。
         * @return T& 可修改的最新状态。
         */
        constexpr T &store_current_state(T new_state = {}) {
            clear_history_behind();
            m_history.push_back(std::move(new_state));
            ++m_position;
            THR_ASSERT_MSG(m_position + 1 == m_history.size(), "`thr::with_history` 内部处理有误。");
            return m_history[m_position];
        }

        /**
         * @brief 获取当前状态。
         * @return const T& 当前状态。
         */
        [[nodiscard]] constexpr const T &get_current_state() const noexcept {
            return m_history[m_position];
        }

        /**
         * @brief 获取可修改的当前状态可修改。
         * @return T& 可修改的当前状态。
         * @note 这个函数会清空当前状态后面的所有历史，若不想清空，请使用无法修改当前状态的 @ref get_current_state。
         */
        [[nodiscard]] constexpr T &get_current_state_modifiable() {
            clear_history_behind();
            return m_history[m_position];
        }

        /**
         * @brief 获取当前状态的索引。
         * @return std::size_t 当前状态的索引。
         */
        [[nodiscard]] constexpr std::size_t get_current_state_index() const noexcept {
            return m_position;
        }

        /**
         * @brief 切换当前状态到指定位置的历史版本。
         * @param [in] position 指定的位置。
         * @warning 请确保 `position` 参数在 `0 ~ m_history.size() - 1` 的范围内，否则行为未定义。
         */
        constexpr void switch_to(std::size_t position) noexcept {
            m_position = position;
            THR_ASSERT_MSG(m_position < m_history.size(),
                           "`position` 参数错误地不在 `0 ~ m_history.size() - 1` 的范围内。");
        }

        /**
         * @brief 切换当前状态到前一个版本。
         * @details 这个函数当 `get_current_state_index() != 0` 时相当于 `switch_to(get_current_state_index() - 1)`，否则无操作。
         */
        constexpr void switch_to_previous() noexcept {
            if (m_position == 0) {
                return;
            }
            --m_position;
        }

        /**
         * @brief 切换当前状态到后一个版本。
         * @details 这个函数当 `get_current_state_index() + 1 != m_history.size()` 时相当于 `switch_to(get_current_state_index() + 1)`，否则无操作。
         */
        constexpr void switch_to_next() noexcept {
            if (m_position + 1 == m_history.size()) {
                return;
            }
            ++m_position;
        }

      private:
        /// @brief 清空后面的历史。
        constexpr void clear_history_behind() {
            m_history.erase(m_history.begin()
                                + static_cast<std::vector<T>::difference_type>(m_position + 1),
                            m_history.end());
        }
    };

} // namespace thr

#endif // THR_BASE_WITH_HISTORY_HPP