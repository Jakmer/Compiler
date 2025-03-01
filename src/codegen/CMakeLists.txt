set(SOURCES
    CodeGenerator.cpp
    InstructNodes.cpp
    Instructions.cpp
    Memory.cpp
)

add_library(codegen_lib
    ${SOURCES}
)

target_include_directories(codegen_lib PUBLIC 
    ${CMAKE_SOURCE_DIR}/inc/codegen
)

target_link_libraries(codegen_lib PUBLIC
    semana_lib
    ast_lib
    compiler_ifc_lib)

add_library(codegen_lib_debug
    ${SOURCES}
)

target_include_directories(codegen_lib_debug PUBLIC 
    ${CMAKE_SOURCE_DIR}/inc/codegen
)

target_link_libraries(codegen_lib_debug PUBLIC
    semana_lib_debug
    ast_lib_debug
    compiler_ifc_lib_debug)

set_target_properties(codegen_lib_debug PROPERTIES
    COMPILE_FLAGS "-ggdb3 -O0"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/debug
)
