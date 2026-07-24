# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 查找当前目录下所有 cpp 文件
file(GLOB_RECURSE SRC_FILES *.cpp)

# 生成库
add_library(thr ${SRC_FILES})

# 查找第三方库
find_package(Lua 5.1 REQUIRED)
if(LUA_VERSION VERSION_GREATER_EQUAL 5.5)
	message(FATAL_ERROR "Lua version ${LUA_VERSION} is too high; need < 5.5")
endif()

CPMAddPackage("gh:skypjack/sol2@3.3.0")
CPMAddPackage("gh:skypjack/entt@3.15.0")
CPMAddPackage(
	NAME
	nlohmann_json
	GITHUB_REPOSITORY
	nlohmann/json
	VERSION
	3.12.0
)
CPMAddPackage(
	NAME
	spdlog
	GITHUB_REPOSITORY
	gabime/spdlog
	VERSION
	1.15.3
	OPTIONS
	"SPDLOG_USE_STD_FORMAT ON"
)
CPMAddPackage(
	NAME
	SFML
	GITHUB_REPOSITORY
	SFML/SFML
	VERSION
	3.0.1
	FIND_PACKAGE_ARGUMENTS
	"COMPONENTS System Window Graphics"
)
CPMAddPackage(
	NAME
	TGUI
	GITHUB_REPOSITORY
	texus/TGUI
	VERSION
	1.10.0
	OPTIONS
	"TGUI_BACKEND SFML_GRAPHICS"
)

# 为 Lua 手动创建目标
if(Lua_FOUND AND NOT TARGET Lua::Lua)
	add_library(Lua::Lua INTERFACE IMPORTED)
	set_target_properties(Lua::Lua
		PROPERTIES
			INTERFACE_INCLUDE_DIRECTORIES "${LUA_INCLUDE_DIR}"
			INTERFACE_LINK_LIBRARIES "${LUA_LIBRARIES}"
	)
endif()

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
		sol2::sol2
		Lua::Lua
)

# 添加安装支持
install(TARGETS thr DESTINATION lib)
