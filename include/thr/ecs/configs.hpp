/**
 * @file configs.hpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief 定义了集中获取一些配置的类。
 * @version 0.1.0-1
 * @date 2026-06-20
 * 
 * @copyright cpp-love
 * 
 */

#ifndef THR_ECS_CONFIGS_HPP
#define THR_ECS_CONFIGS_HPP

#include "thr/base/file.hpp"
#include "thr/base/sfml_helper.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>
#include <TGUI/Font.hpp>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <utility>

namespace thr::ecs {

    /// @brief 集中获取一些配置的类（采用单例模式）。
    class configs {
      public:
        float                         segment_width{};                   ///< 路径宽度。
        sf::Color                     segment_color;                     ///< 路径颜色。
        sf::Color                     segments_render_opacity;           ///< 路径渲染透明度。
        sf::Color                     player_on_ground_color;            ///< 在地面上的玩家颜色。
        sf::Color                     player_under_ground_color;         ///< 在地面下的玩家颜色。
        float                         player_on_ground_side_length{};    ///< 在地面上的玩家边长。
        float                         player_under_ground_side_length{}; ///< 在地面下的玩家边长。
        sf::Color                     background_color;                  ///< 背景颜色。
        std::filesystem::path         font_path;                         ///< 字体路径。
        sf::Vector2u                  game_screen_size;                  ///< 游戏屏幕大小。
        /**
         * @brief 获取 SFML 的字体。
         * @return const sf::Font& SFML 的字体。
         */
        [[nodiscard]] const sf::Font &get_sfml_font() const {
            static sf::Font font{font_path};
            return font;
        };
        /**
         * @brief 获取 TGUI 的字体。
         * @return tgui::Font TGUI 的字体。
         * @todo 不要每次都构造，但是像 `get_sfml_font()` 的方法无法正确处理 TGUI 的生命周期管理。
         */
        [[nodiscard]] tgui::Font get_tgui_font() const { return {font_path.generic_string()}; }

        /// @brief 删除复制构造函数。
        configs(const configs &rhs) = delete;
        /// @brief 删除复制赋值运算符。
        configs &operator=(const configs &rhs) = delete;
        /// @brief 删除移动赋值运算符。
        configs &operator=(configs &&rhs) = delete;
        /// @brief 销毁 configs 对象。
        ~configs() = default;

        static const configs &singleton() {
            static const configs singleton = [] {
                nlohmann::json json;
                std::ifstream  fin(
                    get_existing_full_path("assets/json/configs.json").value() /*实在不行就抛异常爆炸*/);
                fin >> json;
                configs configs{
                    get_existing_full_path(std::filesystem::path("assets/fonts") / json.at("font_path"))
                        .value()};
                configs.segment_width = json.at("segment_width");
                configs.segment_color = json.at("segment_color");
                configs.segments_render_opacity = json.at("segments_render_opacity");
                configs.player_on_ground_color = json.at("player_on_ground_color");
                configs.player_under_ground_color = json.at("player_under_ground_color");
                configs.player_on_ground_side_length = json.at("player_on_ground_side_length");
                configs.player_under_ground_side_length = json.at("player_under_ground_side_length");
                configs.background_color = json.at("background_color");
                configs.game_screen_size = json.at("game_screen_size");
                return configs;
            }();
            return singleton;
        }

      private:
        /**
         * @brief 构建一个 configs 对象。
         * @param [in] font_path 字体路径。
         * @details 为遵循单例模式，将此函数设为私有，以防意外创建多余对象。
         */
        explicit configs(std::filesystem::path the_font_path) : font_path(std::move(the_font_path)) {}

        /**
         * @brief 构建一个 configs 对象。
         * @param [in] rhs 另一个对象。
         * @details 为遵循单例模式，将此函数设为私有，以防意外创建多余对象。
         */
        configs(configs &&rhs) = default;
    };

} // namespace thr::ecs

#endif // THR_ECS_CONFIGS_HPP