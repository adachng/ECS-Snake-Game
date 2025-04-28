cmake_minimum_required(VERSION 3.22)
project(Snake-Game-CLI
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17) # because of EnTT; https://skypjack.github.io/entt/#requirements
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>") # for all executables
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>") # for libSDL3.so etc.

add_subdirectory(third_party)
add_subdirectory(src)
add_subdirectory(spike)
