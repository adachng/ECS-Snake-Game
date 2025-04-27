cmake_minimum_required(VERSION 3.22)
project(Snake-Game-CLI
    LANGUAGES CXX
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>") # for all executables
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>") # for libSDL3.so etc.

add_subdirectory(extern)
add_subdirectory(src)
add_subdirectory(spike)
