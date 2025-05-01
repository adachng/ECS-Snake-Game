enable_testing()
link_libraries(GTest::gtest_main)

add_executable(main_test main_test.cpp)
target_include_directories(main_test PRIVATE "${CMAKE_SOURCE_DIR}/src")

include(GoogleTest)
gtest_discover_tests(main_test)
