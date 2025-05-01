set(MAIN_TARGET snake_game)

add_executable(${MAIN_TARGET} main.cpp)
target_link_libraries(${MAIN_TARGET} PRIVATE SDL3::SDL3)
