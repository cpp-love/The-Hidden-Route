/**
 * @file concepts.hpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 定义一些标准库没有的概念和 type traits
 * @version 0.1.0-1
 * @date 2026-05-01
 * 
 * @copyright cpp-love
 * 
 */

#ifndef THR_BASE_CONCEPTS_HPP
#define THR_BASE_CONCEPTS_HPP

#include <type_traits>
#include <variant>

namespace thr {

    /**
     * @brief 类型萃取：类型 `T` 是否为 变体( `std::variant` )类型 `V` 的成员
     * @tparam Variant 变体类型
     * @tparam T 判断类型
     */
    template <typename Variant, typename T>
    struct is_variant_member : std::false_type {};
    template <typename T1, typename... Rest, typename T>
    struct is_variant_member<std::variant<T1, Rest...>, T>
        : std::conditional_t<std::is_same_v<T, T1>, std::true_type,
                             is_variant_member<std::variant<Rest...>, T>> {};

    /**
     * @brief 类型萃取： @ref is_variant_member 的值的模板缩写
     * @tparam Variant 变体类型
     * @tparam T 判断类型
     */
    template <typename Variant, typename T>
    constexpr bool is_variant_member_v = is_variant_member<Variant, T>::value;

    template <typename Variant, typename T>
    concept variant_member = is_variant_member_v<Variant, T>;

} // namespace thr

#endif // THR_BASE_CONCEPTS_HPP