option(THR_BUILD_TESTS "Build The Non-\"real\" World tests" OFF)

# 添加测试
enable_testing()

# 默认不添加，如果启用 THR_BUILD_TESTS 则默认添加
if(THR_BUILD_TESTS)
	option(THR_BUILD_BASE_TESTS "Build the tests in the tests/base directory" ON)
	option(THR_BUILD_ECS_TESTS "Build the tests in the tests/ecs directory" ON)
else()
	option(THR_BUILD_BASE_TESTS "Build the tests in the tests/base directory" OFF)
	option(THR_BUILD_ECS_TESTS "Build the tests in the tests/ecs directory" OFF)
endif()

function(build_test_executable sources)
	foreach(source IN LISTS sources)
		# 构建程序

		cmake_path(GET source STEM target_name)
		list(APPEND target_names ${target_name})
		# 设置链接文件
		add_executable(${target_name} ${source})
		# 链接第三方库
		target_link_libraries(${target_name}
			PUBLIC
				thr_compiler_flags
				thr
		)
	endforeach()

	# 添加安装支持
	install(
		TARGETS ${target_names}
		DESTINATION bin/tests
	)
endfunction()

if(THR_BUILD_BASE_TESTS)
	# 设置输出目录
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests)

	# 构建程序
	set(build_list
		${CMAKE_CURRENT_SOURCE_DIR}/thr/base/test_assert_msg.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/thr/base/test_floating_point_compare.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/thr/base/test_sfml_helper.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/thr/base/test_overload.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/thr/base/test_observable.cpp
	)
	build_test_executable("${build_list}")
	unset(build_list)

	# 添加测试支持
	add_test(NAME RunTestBase3 COMMAND test_floating_point_compare)
	add_test(NAME RunTestBase4 COMMAND test_sfml_helper)
	add_test(NAME TestBase4StandardUse COMMAND test_sfml_helper)
	set_tests_properties(TestBase4StandardUse
		PROPERTIES PASS_REGULAR_EXPRESSION "vec1 is \\(10.000, 10.000\\), vec2 is \\(11, 11\\), vec3 is \\(13, 10\\)\\s*"
	)
	add_test(NAME RunTestBase5 COMMAND test_overload)
	add_test(NAME TestBase5StandardUse COMMAND test_overload)
	set_tests_properties(TestBase5StandardUse
		PROPERTIES PASS_REGULAR_EXPRESSION "value: 2\ncall from lambda\nvalue: 1\ncall from lambda3\\s*"
	)
	add_test(NAME RunTestBase6 COMMAND test_observable)
	add_test(NAME TestBase6StandardUse COMMAND test_observable)
	set_tests_properties(TestBase6StandardUse
		PROPERTIES PASS_REGULAR_EXPRESSION "value changed, now is: 11\nvalue changed, now is: 22\\s*"
	)

	# todo : 解决下面的测试失败的问题
	# add_test(NAME RunTestBase2 COMMAND test_assert_msg)
	# set_tests_properties(RunTestBase2 PROPERTIES WILL_FAIL TRUE)
endif()

if(THR_BUILD_ECS_TESTS)
	# 设置输出目录
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests)

	# 构建程序
	set(build_list
		${CMAKE_CURRENT_SOURCE_DIR}/thr/ecs/systems/test_scene_system.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/thr/ecs/systems/test_level_serialization_system.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/thr/ecs/systems/test_level_render_system.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/thr/ecs/systems/test_level_graph_render_system.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/thr/ecs/systems/test_level_graph_serialization_system.cpp
	)
	build_test_executable("${build_list}")
	unset(build_list)

	# 添加测试支持
	add_test(NAME RunTestEcs2 COMMAND test_scene_system)
	# todo : 修复此错误
	# add_test(NAME TestEcs2StandardUse COMMAND test_scene_system)
	# set_tests_properties(TestEcs2StandardUse
	# PROPERTIES PASS_REGULAR_EXPRESSION "创建场景1\n将孩子1添加到场景1中\n现在孩子1的父亲有：\n\\d+\n将孩子2添加到场景1中\n现在场景1的子实体有：\n\\d+ \\d+\n将孩子1从场景1中删除\n现在孩子1的父亲有：\n创建场景2\n现在registry拥有场景：\nid: 1 entity: \\d+\nid: 2 entity: \\d+\n销毁场景2\n现在registry拥有场景：\nid: 1 entity: \\d+\n删除所有场景\n现在registry拥有场景：\\s*")
	# add_test(NAME RunTestEcs3 COMMAND test_level_serialization_system)
	add_test(NAME RunTestEcs4 COMMAND test_level_graph_serialization_system)
	add_test(
		NAME RunTestEcs5
		COMMAND test_level_graph_render_system
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	)

endif()
