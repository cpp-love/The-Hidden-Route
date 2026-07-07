# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 查找当前目录下所有 cpp 文件
file(GLOB_RECURSE SRC_FILES *.cpp)

# 生成库
add_library(thr ${SRC_FILES})

# 查找第三方库
include(FetchContent)

FetchContent_Declare(
    EnTT
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG origin/main
    FIND_PACKAGE_ARGS 3.15.0 CONFIG QUIET
)
FetchContent_MakeAvailable(EnTT)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG origin/v1.x
    FIND_PACKAGE_ARGS 1.15.3 CONFIG QUIET
)
set(SPDLOG_USE_STD_FORMAT ON)
FetchContent_MakeAvailable(spdlog)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG origin/master
    FIND_PACKAGE_ARGS 3.0.1 COMPONENTS Graphics Window System CONFIG QUIET
)
FetchContent_MakeAvailable(SFML)
FetchContent_Declare(
    TGUI
    GIT_REPOSITORY https://github.com/texus/tgui.git
    GIT_TAG origin/1.x
    FIND_PACKAGE_ARGS 1.10.0 CONFIG QUIET
)
set(TGUI_BACKEND SFML_GRAPHICS)
FetchContent_MakeAvailable(TGUI)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG origin/develop
    FIND_PACKAGE_ARGS 3.12.0 CONFIG QUIET
)
FetchContent_MakeAvailable(nlohmann_json)

# 链接编译器标志
target_link_libraries(thr
    PUBLIC
    thr_compiler_flags
    EnTT::EnTT
    spdlog::spdlog
    SFML::Graphics
    SFML::Window
    SFML::System
    TGUI::TGUI
    nlohmann_json::nlohmann_json
)

# 添加安装支持
install(TARGETS thr DESTINATION lib)
