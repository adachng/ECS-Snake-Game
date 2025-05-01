enable_testing()
link_libraries(GTest::gtest_main)

add_executable(googletest_factorial googletest_factorial.cpp)

include(GoogleTest)
gtest_discover_tests(googletest_factorial)
