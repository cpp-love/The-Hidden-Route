# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 设置链接文件
add_executable(The-Hidden-Route
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/game_states.cpp
)

# 包含头文件目录
target_include_directories(The-Hidden-Route
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

# 链接第三方库
target_link_libraries(The-Hidden-Route
	PUBLIC
		thr_compiler_flags
		thr
)

# 添加安装支持
install(TARGETS The-Hidden-Route DESTINATION bin)
