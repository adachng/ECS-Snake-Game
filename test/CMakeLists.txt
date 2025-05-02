enable_testing()

add_executable(main_test
    main_test.cpp
    translate_2d_test.cpp
    snake_gameplay_test.cpp
)
target_link_libraries(main_test PRIVATE
    GTest::gtest
    ${CMAKE_PROJECT_NAME}::system
)

include(GoogleTest)
gtest_discover_tests(main_test)
