cmake_minimum_required(VERSION 3.22.1)

project(Compiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(grammar)
add_subdirectory(src/ast)
add_subdirectory(src/semantic_analyzer)

include_directories(inc)
include_directories(grammar)
include_directories(${CMAKE_BINARY_DIR})

set(SOURCES
    src/app/main.cpp
)

add_executable(compiler ${SOURCES})

target_link_libraries(compiler
    grammar
    ast_lib
    semana_lib
)

add_executable(compiler_debug ${SOURCES})

target_link_libraries(compiler_debug
    grammar_debug
    ast_lib_debug
    semana_lib_debug
)

set_target_properties(compiler_debug PROPERTIES
    COMPILE_FLAGS "-ggdb3 -O0"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/debug
)

add_custom_target(test
    COMMAND ${CMAKE_SOURCE_DIR}/test/start_testing.sh ${CMAKE_SOURCE_DIR}
    DEPENDS compiler
)
