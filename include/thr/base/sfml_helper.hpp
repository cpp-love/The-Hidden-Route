/**
 * @file sfml_helper.hpp
 * @author cpp-love (15865418+cpp-love@user.noreply.gitee.com)
 * @brief 添加第三方库 SFML 对其他库/标准库的支持。
 * @version 0.1.0-2
 * @date 2026-06-20
 * 
 * @copyright cpp-love
 * 
 */

#ifndef THR_BASE_SFML_HELPER_HPP
#define THR_BASE_SFML_HELPER_HPP

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <format>
#include <locale>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

namespace std {

    /**
     * @brief `std::formatter` 对 `sf::Vector2` 的偏特化，用于格式化
     * @tparam T `sf::Vector2` 的模板参数
     * @tparam CharT `std::formatter` 的输出字符参数
     * @details 格式化参数使用 `T` 类型的格式化参数，来格式化成员 `x` 和 `y`，
     *          格式化结果为： `(x, y)`
     */
    template <typename T, typename CharT>
    struct formatter<sf::Vector2<T>, CharT> {
        using fmt_type = sf::Vector2<T>; ///< 格式化参数
        using char_type = CharT;         ///< 字符类型
        std::formatter<T, char_type> m_formatter_impl;
        /**
         * @brief 解析格式化参数的解析器
         * @tparam ParseCtx 解析的上下文类型
         * @param [in] ctx 上下文
         * @return ParseCtx::iterator 解析后的迭代器
         */
        template <typename ParseCtx>
        constexpr ParseCtx::iterator parse(ParseCtx &ctx) {
            return m_formatter_impl.parse(ctx);
        }
        /**
         * @brief 格式化器
         * @tparam ParseCtx 格式化的上下文类型
         * @param [in] vector 要格式化的对象
         * @param [in, out] ctx 上下文
         * @return FmtCtx::iterator 格式化后的迭代器
         */
        template <typename FmtCtx>
        typename FmtCtx::iterator format(const fmt_type &vector, FmtCtx &ctx) const {
            const auto &facet = std::use_facet<std::ctype<char_type>>(ctx.locale());
            auto        out = ctx.out();
            *out = facet.widen('(');
            ++out;
            ctx.advance_to(out);
            out = m_formatter_impl.format(vector.x, ctx);
            *out = facet.widen(',');
            ++out;
            *out = facet.widen(' ');
            ++out;
            ctx.advance_to(out);
            out = m_formatter_impl.format(vector.y, ctx);
            *out = facet.widen(')');
            ++out;
            return out;
        }
    };

    /**
     * @brief `std::formatter` 对 `sf::Vector3` 的偏特化，用于格式化
     * @tparam T `sf::Vector3` 的模板参数
     * @tparam CharT `std::formatter` 的输出字符参数
     * @details 格式化参数使用 `T` 类型的格式化参数，来格式化成员 `x`、`y` 和 `z`，
     *          格式化结果为： `(x, y, z)`
     */
    template <typename T, typename CharT>
    struct formatter<sf::Vector3<T>, CharT> {
        using fmt_type = sf::Vector3<T>; ///< 格式化参数
        using char_type = CharT;         ///< 字符类型
        std::formatter<T, CharT> m_formatter_impl;
        /**
         * @brief 解析格式化参数的解析器
         * @tparam ParseCtx 解析的上下文类型
         * @param [in] ctx 上下文
         * @return ParseCtx::iterator 解析后的迭代器
         */
        template <typename ParseCtx>
        constexpr ParseCtx::iterator parse(ParseCtx &ctx) {
            return m_formatter_impl.parse(ctx);
        }
        /**
         * @brief 格式化器
         * @tparam ParseCtx 格式化的上下文类型
         * @param [in] vector 要格式化的对象
         * @param [in, out] ctx 上下文
         * @return FmtCtx::iterator 格式化后的迭代器
         */
        template <typename FmtCtx>
        typename FmtCtx::iterator format(const fmt_type &vector, FmtCtx &ctx) const {
            auto facet = std::use_facet<std::ctype<char_type>>(ctx.locale());
            auto out = ctx.out();
            *out = facet.widen('(');
            ++out;
            ctx.advance_to(out);
            out = m_formatter_impl.format(vector.x, ctx);
            *out = facet.widen(',');
            ++out;
            *out = facet.widen(' ');
            ++out;
            ctx.advance_to(out);
            out = m_formatter_impl.format(vector.y, ctx);
            *out = facet.widen(',');
            ++out;
            *out = facet.widen(' ');
            ++out;
            ctx.advance_to(out);
            out = m_formatter_impl.format(vector.z, ctx);
            *out = facet.widen(')');
            ++out;
            return out;
        }
    };

} // namespace std

NLOHMANN_JSON_NAMESPACE_BEGIN

/// @brief `nlohmann::adl_serializer` 对 `sf::Color` 的特化，用于序列化。
template <>
struct adl_serializer<sf::Color> {

    /**
     * @brief 序列化颜色。
     * @param [out] json 存储序列化结果的 json。
     * @param [in] color 要序列化的颜色。
     * @details 
     * - 序列化格式说明：
     *   - 永远采用反序列化的第 4 种格式（即 `sf::Color(<color_integer>)` 对应 `<color_integer>`）。
     */
    static void to_json(json &json, const sf::Color &color) { json = color.toInteger(); }

    /**
     * @brief 反序列化颜色。
     * @param [in] json 存储颜色的 json。
     * @param [out] color 存储反序列化结果的颜色。
     * @details 
     * - 反序列化格式说明：
     *   1. `"<color_string>"`                         对应 `sf::Color::<color_string>`
     *      - 注意： json `"<color_string>"` 中的首字母为小写，而 `sf::Color::<color_string>` 中的首字母为大写！
     *   2. `{"r": <r>, "g": <g>, "b": <b>}`           对应 `sf::Color(<r>, <g>, <b>)`
     *   3. `{"r": <r>, "g": <g>, "b": <b>, "a": <a>}` 对应 `sf::Color(<r>, <g>, <b>, <a>)`
     *   4. `<color_unsigned_integer>`                 对应 `sf::Color(<color_unsigned_integer>)`
     */
    static void from_json(const json &json, sf::Color &color) {
        if (json.is_string()) {
            static const std::unordered_map<std::string, sf::Color> map_for_converting{
                {"black", sf::Color::Black},
                {"blue", sf::Color::Blue},
                {"cyan", sf::Color::Cyan},
                {"green", sf::Color::Green},
                {"magenta", sf::Color::Magenta},
                {"red", sf::Color::Red},
                {"transparent", sf::Color::Transparent},
                {"white", sf::Color::White},
                {"yellow", sf::Color::Yellow},
            };
            auto iter = map_for_converting.find(json);
            if (iter == map_for_converting.end()) {
                throw nlohmann::json::out_of_range::create(411, "json value is not in the allowed list",
                                                           &json);
            }
            color = iter->second;
            return;
        }
        if (json.is_number_unsigned()) {
            color = sf::Color(json.get<std::uint32_t>());
            return;
        }
        if (json.is_object()) {
            color = sf::Color(json.at("r").get<std::uint8_t>(), json.at("g").get<std::uint8_t>(),
                              json.at("b").get<std::uint8_t>(), json.value("a", 255));
            return;
        }
        throw nlohmann::json::type_error::create(
            302,
            std::format("type must be string, unsigned int, or object, but is {}", json.type_name()),
            &json);
    }
};

/**
 * @brief `nlohmann::adl_serializer` 对 `sf::Vector2` 的偏特化，用于序列化。
 * @tparam T `sf::Vector2` 的模板参数。
 */
template <typename T>
struct adl_serializer<sf::Vector2<T>> {

    /**
     * @brief 序列化向量。
     * @param [out] json 存储序列化结果的 json。
     * @param [in] vector 要序列化的向量。
     * @details 
     * - 序列化格式说明：
     *   - `sf::Vector2(<x>, <y>)` 对应 `{"x": <x>, "y": <y>}`
     */
    static void to_json(json &json, const sf::Vector2<T> &vector) {
        json = {{"x", vector.x}, {"y", vector.y}};
    }

    /**
     * @brief 反序列化向量。
     * @param [in] json 存储向量的 json。
     * @param [out] vector 存储反序列化结果的向量。
     * @details 
     * - 反序列化格式说明：
     *   - `{"x": <x>, "y": <y>}` 对应 `sf::Vector2(<x>, <y>)`
     */
    static void from_json(const json &json, sf::Vector2<T> &vector) {
        if (json.is_object()) {
            vector = sf::Vector2<T>(json.at("x").get<T>(), json.at("y").get<T>());
            return;
        }
        throw nlohmann::json::type_error::create(
            302, std::format("type must be object, but is {}", json.type_name()), &json);
    }
};

/**
 * @brief `nlohmann::adl_serializer` 对 `sf::Vector3` 的偏特化，用于序列化。
 * @tparam T `sf::Vector3` 的模板参数。
 */
template <typename T>
struct adl_serializer<sf::Vector3<T>> {

    /**
     * @brief 序列化向量。
     * @param [out] json 存储序列化结果的 json。
     * @param [in] vector 要序列化的向量。
     * @details 
     * - 序列化格式说明：
     *   - `sf::Vector3(<x>, <y>, <z>)` 对应 `{"x": <x>, "y": <y>, "z": <z>}`
     */
    static void to_json(json &json, const sf::Vector3<T> &vector) {
        json = {{"x", vector.x}, {"y", vector.y}, {"z", vector.z}};
    }

    /**
     * @brief 反序列化向量。
     * @param [in] json 存储向量的 json。
     * @param [out] vector 存储反序列化结果的向量。
     * @details 
     * - 反序列化格式说明：
     *   - `{"x": <x>, "y": <y>, "z": <z>}` 对应 `sf::Vector3(<x>, <y>, <z>)`
     */
    static void from_json(const json &json, sf::Vector3<T> &vector) {
        if (json.is_object()) {
            vector = sf::Vector3<T>(json.at("x").get<T>(), json.at("y").get<T>(), json.at("z").get<T>());
            return;
        }
        throw nlohmann::json::type_error::create(
            302, std::format("type must be object, but is {}", json.type_name()), &json);
    }
};

NLOHMANN_JSON_NAMESPACE_END

#endif // THR_BASE_SFML_HELPER_HPP