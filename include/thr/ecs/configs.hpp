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
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <optional>

namespace thr::ecs {

    /// @brief 集中获取一些配置的类（采用单例模式）。
    class configs {
      public:
        float     segment_width{};                   ///< 路径宽度。
        sf::Color segment_color;                     ///< 路径颜色。
        sf::Color segments_render_opacity{};         ///< 路径渲染透明度。
        sf::Color player_on_ground_color;            ///< 在地面上的玩家颜色。
        sf::Color player_under_ground_color;         ///< 在地面下的玩家颜色。
        float     player_on_ground_side_length{};    ///< 在地面上的玩家边长。
        float     player_under_ground_side_length{}; ///< 在地面下的玩家边长。
        sf::Color background_color;                  ///< 背景颜色。

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
                configs        configs;
                nlohmann::json json;
                std::ifstream  fin(
                    get_executable_directory()
                        .and_then(
                            [](std::filesystem::path path) -> std::optional<std::filesystem::path> {
                                // 先尝试可执行文件父文件夹目录下的配置文件。
                                path /= "configs.json";
                                if (std::filesystem::exists(path)) {
                                    return path;
                                }
                                return std::nullopt;
                            })
                        .or_else([] -> std::optional<std::filesystem::path> {
                            // 再尝试工作目录下的配置文件。
                            std::filesystem::path path =
                                std::filesystem::current_path() / "configs.json";
                            if (std::filesystem::exists(path)) {
                                return path;
                            }
                            return std::nullopt;
                        })
                        .value() /*实在不行就抛异常爆炸*/);
                fin >> json;
                configs.segment_width = json["segment_width"];
                configs.segment_color = json["segment_color"];
                configs.segments_render_opacity = json["segments_render_opacity"];
                configs.player_on_ground_color = json["player_on_ground_color"];
                configs.player_under_ground_color = json["player_under_ground_color"];
                configs.player_on_ground_side_length = json["player_on_ground_side_length"];
                configs.player_under_ground_side_length = json["player_under_ground_side_length"];
                configs.background_color = json["background_color"];
                return configs;
            }();
            return singleton;
        }

      private:
        /**
         * @brief 构建一个 configs 对象。
         * @details 为遵循单例模式，将此函数设为私有，以防意外创建对象。
         */
        constexpr configs() = default;
        /**
         * @brief 构建一个 configs 对象。
         * @param [in] rhs 另一个对象。
         * @details 为遵循单例模式，将此函数设为私有，以防意外创建对象。
         */
        configs(configs &&rhs) = default;
    };

} // namespace thr::ecs

#endif // THR_ECS_CONFIGS_HPP