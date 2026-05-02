option(THR_BUILD_TESTS "Build The Non-\"real\" World tests" OFF)

# 添加测试
enable_testing()

# 默认不添加，如果启用 THR_BUILD_TESTS 则默认添加
if(THR_BUILD_TESTS)
    option(THR_BUILD_BASE_TESTS "Build the tests in the tests/base directory" ON)
else()
    option(THR_BUILD_BASE_TESTS "Build the tests in the tests/base directory" OFF)
endif()

if(THR_BUILD_BASE_TESTS)
    # 设置输出目录
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests)

    # 设置链接文件
    add_executable(test_assert_msg ${CMAKE_CURRENT_SOURCE_DIR}/thr/base/test_assert_msg.cpp)
    add_executable(test_floating_point_compare ${CMAKE_CURRENT_SOURCE_DIR}/thr/base/test_floating_point_compare.cpp)
    add_executable(test_SFML_formatter ${CMAKE_CURRENT_SOURCE_DIR}/thr/base/test_SFML_formatter.cpp)
    add_executable(test_overload ${CMAKE_CURRENT_SOURCE_DIR}/thr/base/test_overload.cpp)
    add_executable(test_observable ${CMAKE_CURRENT_SOURCE_DIR}/thr/base/test_observable.cpp)

    # 链接第三方库
    target_link_libraries(test_assert_msg
        PUBLIC
        thr_compiler_flags
        thr
    )

    target_link_libraries(test_floating_point_compare
        PUBLIC
        thr_compiler_flags
        thr
    )

    target_link_libraries(test_SFML_formatter
        PUBLIC
        thr_compiler_flags
        thr
    )

    target_link_libraries(test_overload
        PUBLIC
        thr_compiler_flags
        thr
    )

    target_link_libraries(test_observable
        PUBLIC
        thr_compiler_flags
        thr
    )

    # 添加安装支持
    install(
        TARGETS test_assert_msg test_floating_point_compare
        test_SFML_formatter test_overload test_observable
        DESTINATION bin/tests)

    # 添加测试支持
    add_test(NAME RunTestBase3 COMMAND test_floating_point_compare)
    add_test(NAME RunTestBase4 COMMAND test_SFML_formatter)
    add_test(NAME TestBase4StandardUse COMMAND test_SFML_formatter)
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
