set(MAIN_TARGET snake_game)

add_subdirectory(component)
add_subdirectory(system)

add_executable(${MAIN_TARGET} main.cpp)
add_executable(${CMAKE_PROJECT_NAME}::${MAIN_TARGET} ALIAS ${MAIN_TARGET})
target_link_libraries(${MAIN_TARGET} PRIVATE
    SDL3::SDL3
    ${CMAKE_PROJECT_NAME}::component
    ${CMAKE_PROJECT_NAME}::system
)
