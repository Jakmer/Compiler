set(SOURCES
    CodeGenerator.cpp
)

add_library(codegen_lib
    ${SOURCES}
)

target_include_directories(codegen_lib PUBLIC 
    ${CMAKE_SOURCE_DIR}/inc/codegen
    ${CMAKE_SOURCE_DIR}/inc/semantic_analyzer
    ${CMAKE_SOURCE_DIR}/inc/compiler_ifc
    ${CMAKE_SOURCE_DIR}/inc/ast
)

add_library(codegen_lib_debug
    ${SOURCES}
)

target_include_directories(codegen_lib_debug PUBLIC 
    ${CMAKE_SOURCE_DIR}/inc/codegen
    ${CMAKE_SOURCE_DIR}/inc/semantic_analyzer
    ${CMAKE_SOURCE_DIR}/inc/compiler_ifc
    ${CMAKE_SOURCE_DIR}/inc/ast
)

set_target_properties(codegen_lib_debug PROPERTIES
    COMPILE_FLAGS "-ggdb3 -O0"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/debug
)
