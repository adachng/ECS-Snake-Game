enable_testing()
link_libraries(GTest::gtest_main)

add_executable(main_test main_test.cpp)

include_directories("${CMAKE_SOURCE_DIR}/src")

add_executable(translate_2d_test translate_2d_test.cpp)
target_link_libraries(translate_2d_test PRIVATE ${CMAKE_PROJECT_NAME}::system)

include(GoogleTest)
gtest_discover_tests(
    main_test
    translate_2d_test
)
