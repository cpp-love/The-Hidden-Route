/**
 * @file main.cpp
 * @author cpp-love (207296385+cpp-love@users.noreply.github.com)
 * @brief *The Hidden Route* 游戏的主程序。
 * @version 0.1.0-3
 * @date 2026-07-12
 * 
 * @copyright cpp-love
 * 
 */

#include "game_states.hpp"
#include "thr/ecs/configs.hpp"
#include "thr/ecs/systems/global/game_state_manager.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <memory>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

int main() {

#ifdef _WIN32
    // 让Windows支持UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif // _WIN32

    // spdlog config
    auto file_logger = std::make_shared<spdlog::sinks::basic_file_sink_st>("log/log.log");
    file_logger->set_level(spdlog::level::trace);
    auto stdout_logger = std::make_shared<spdlog::sinks::stdout_sink_st>();
    stdout_logger->set_level(spdlog::level::info);
    auto multi_sink_logger = std::make_shared<spdlog::logger>(
        "multi_sink_logger", spdlog::sinks_init_list{file_logger, stdout_logger});
    multi_sink_logger->set_level(spdlog::level::trace);
    spdlog::set_default_logger(multi_sink_logger);
    spdlog::flush_on(spdlog::level::info);

    sf::RenderWindow             window(sf::VideoMode(thr::ecs::configs::singleton().game_screen_size),
                                        "The Hidden Route");
    thr::ecs::game_state_manager manager(window);
    manager.push_state(std::make_unique<mainhelper::main_menu>());

    auto prev = thr::ecs::clock::now();
    while (true) {
        // handle event
        while (const std::optional event = window.pollEvent()) { manager.handle_event(*event); }
        if (!window.isOpen()) {
            break;
        }

        // update
        auto cur = thr::ecs::clock::now();
        manager.update(cur - prev);
        prev = cur;

        // render
        window.clear(thr::ecs::configs::singleton().background_color);
        manager.draw();
        window.display();
    }

    return 0;
}