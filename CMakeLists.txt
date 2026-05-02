cmake_minimum_required(VERSION 3.28)
project(The-Hidden-Route VERSION 0.1.0 DESCRIPTION "迷宫，但是被隐藏的路由你定义。" LANGUAGES CXX)

# 取消编译器扩展
set(CMAKE_CXX_EXTENSIONS OFF "Disable C++ compiler extensions")

# 取消模块
set(CMAKE_CXX_SCAN_FOR_MODULES OFF "Disable CMake C++ modules support")

# 添加测试
enable_testing()

# 添加版本
configure_file(${PROJECT_SOURCE_DIR}/include/thr/base/config.hpp.in ${PROJECT_SOURCE_DIR}/include/thr/base/config.hpp)
configure_file(${PROJECT_SOURCE_DIR}/Doxyfile.internal.in ${PROJECT_SOURCE_DIR}/Doxyfile.internal)
configure_file(${PROJECT_SOURCE_DIR}/Doxyfile.in ${PROJECT_SOURCE_DIR}/Doxyfile)

# 添加编译器标志
add_library(thr_compiler_flags INTERFACE)

# 添加编译器版本
target_compile_features(thr_compiler_flags INTERFACE cxx_std_23)

# 添加编译器警告标志
set(gcc_like_cxx $<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>)
set(msvc_cxx $<COMPILE_LANG_AND_ID:CXX,MSVC>)
target_compile_options(thr_compiler_flags
    INTERFACE
    $<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wpedantic>>
    $<${msvc_cxx}:$<BUILD_INTERFACE:-W3>>
)

# 添加优化
target_compile_options(thr_compiler_flags
    INTERFACE
    $<${gcc_like_cxx}:$<BUILD_INTERFACE:$<$<CONFIG:Release>:-O2>>>
)

# 添加gcc对c++23 `print` / `println` 函数的 `Windows` 版本的支持
set(gcc_cxx $<COMPILE_LANG_AND_ID:CXX,GNU>)
if(gcc_cxx)
    target_link_libraries(thr_compiler_flags
        INTERFACE
        $<BUILD_INTERFACE:-lstdc++exp>
    )
endif()

# 包含头文件目录

target_include_directories(thr_compiler_flags
    INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

# 添加调试/发行宏
target_compile_definitions(thr_compiler_flags
    INTERFACE
    $<BUILD_INTERFACE:$<$<CONFIG:Debug>:DEBUG=1>>
    $<BUILD_INTERFACE:$<$<CONFIG:Debug>:_DEBUG=1>>
    $<BUILD_INTERFACE:$<$<CONFIG:Release>:NDEBUG=1>>
)


# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 添加子目录
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(helper)
