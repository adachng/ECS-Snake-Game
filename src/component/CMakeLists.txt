add_library(component INTERFACE)
add_library(${CMAKE_PROJECT_NAME}::component ALIAS component)

target_include_directories(component INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_link_libraries(component INTERFACE SDL3::SDL3)
