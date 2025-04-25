set(MAIN_TARGET ${PROJECT_NAME})

add_executable(${MAIN_TARGET} main.cpp)

set_target_properties(${MAIN_TARGET} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
