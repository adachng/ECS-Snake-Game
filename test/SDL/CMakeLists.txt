add_executable(SDL-Hello-World sdl-hello-world.cpp)
target_link_libraries(SDL-Hello-World PRIVATE SDL3::SDL3)

add_executable(SDL-Keyboard sdl-keyboard.cpp)
target_link_libraries(SDL-Keyboard PRIVATE SDL3::SDL3)
