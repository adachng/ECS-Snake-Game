cmake_minimum_required(VERSION 3.22)
project(Snake-Game-CPP
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17) # because of EnTT; https://skypjack.github.io/entt/#requirements
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG main # live at head: https://github.com/google/googletest?tab=readme-ov-file#live-at-head
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>") # for all executables
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>") # for libSDL3.so etc.

add_subdirectory(third_party)
add_subdirectory(src)
add_subdirectory(spike)
