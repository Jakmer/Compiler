set(SOURCES
    CompilerInterface.cpp 
    Context.cpp
)

add_library(compiler_ifc_lib
    ${SOURCES}
)

target_include_directories(compiler_ifc_lib PUBLIC 
    ${CMAKE_SOURCE_DIR}/inc/compiler_ifc
)

target_link_libraries(compiler_ifc_lib PUBLIC
    semana_lib
    codegen_lib)

add_library(compiler_ifc_lib_debug
    ${SOURCES}
)

target_include_directories(compiler_ifc_lib_debug PUBLIC 
    ${CMAKE_SOURCE_DIR}/inc/compiler_ifc
)

target_link_libraries(compiler_ifc_lib_debug PUBLIC
    semana_lib_debug
    codegen_lib_debug)

set_target_properties(compiler_ifc_lib_debug PROPERTIES
    COMPILE_FLAGS "-ggdb3 -O0"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/debug
)
