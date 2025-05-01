link_libraries(${CMAKE_PROJECT_NAME}::component)

add_library(system INTERFACE)
add_library(${CMAKE_PROJECT_NAME}::system ALIAS system)

target_include_directories(system INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_link_libraries(system INTERFACE
    SDL3::SDL3
    EnTT::EnTT
    Pal::Sigslot
)
