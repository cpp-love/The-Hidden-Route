option(THR_BUILD_HELPER "Build The Non-\"real\" World helper programs" OFF)

# 添加测试
enable_testing()

if(THR_BUILD_HELPER)
	# 设置输出目录
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/helper)

	# 设置链接文件
	add_executable(generate_file_version
		${CMAKE_CURRENT_SOURCE_DIR}/generate_file_version.cpp
	)
	add_executable(generate_level
		${CMAKE_CURRENT_SOURCE_DIR}/generate_level.cpp
	)

	# 链接第三方库
	target_link_libraries(generate_file_version
		PUBLIC thr_compiler_flags
	)
	target_link_libraries(generate_level
		PUBLIC
			thr_compiler_flags
			thr
	)

	# 添加安装支持
	install(
		TARGETS
			generate_file_version
			generate_level
		RUNTIME DESTINATION bin/helper COMPONENT Helper
	)

	install(CODE "
		include(BundleUtilities)
		include(InstallRequiredSystemLibraries)

		fixup_bundle(\"\${CMAKE_INSTALL_PREFIX}/bin/helper/generate_file_version.exe\" \"\" \"\")
		fixup_bundle(\"\${CMAKE_INSTALL_PREFIX}/bin/helper/generate_level.exe\" \"\" \"\")
		"
		COMPONENT
		Helper
	)

	install(
		PROGRAMS
			generate_code_issues.py
			generate_test_expressions_data.py
			helper_base.py
			move_source_file.py
		DESTINATION bin/helper
		COMPONENT Helper
	)

	# 添加测试支持
	add_test(
		NAME RunHelper
		COMMAND generate_file_version ${PROJECT_SOURCE_DIR}
			${PROJECT_SOURCE_DIR}/include default_author
	)
endif()
