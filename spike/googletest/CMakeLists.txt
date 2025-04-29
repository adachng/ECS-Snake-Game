enable_testing()

add_executable(
    gtest_factorial
    gtest_factorial.cpp
)
target_link_libraries(
    gtest_factorial
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(gtest_factorial)
