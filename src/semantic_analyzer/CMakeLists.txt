set(SOURCES
    SemanticAnalyzer.cpp
    SymbolTable.cpp
    Symbol.cpp
    ErrorMessages.cpp
)

add_library(semana_lib
    ${SOURCES}
)

target_include_directories(semana_lib PUBLIC 
    ${CMAKE_SOURCE_DIR}/inc/semantic_analyzer
)

target_link_libraries(semana_lib PUBLIC
    ast_lib
    compiler_ifc_lib)

add_library(semana_lib_debug 
    ${SOURCES}
)

target_include_directories(semana_lib_debug PUBLIC 
    ${CMAKE_SOURCE_DIR}/inc/semantic_analyzer
)

target_link_libraries(semana_lib_debug PUBLIC
    ast_lib_debug
    compiler_ifc_lib_debug)

set_target_properties(semana_lib_debug PROPERTIES
    COMPILE_FLAGS "-ggdb3 -O0"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/debug
)
