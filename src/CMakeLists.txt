set(MAIN_TARGET ${PROJECT_NAME})

add_executable(${MAIN_TARGET} main.cpp)
target_link_libraries(${MAIN_TARGET} PRIVATE SDL3::SDL3)
