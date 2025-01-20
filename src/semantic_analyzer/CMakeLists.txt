set(SOURCES
    SemanticAnalyzer.cpp
    SymbolTable.cpp
    ErrorMessages.cpp
)

add_library(semana_lib
    ${SOURCES}
)

target_include_directories(semana_lib PUBLIC 
  ${CMAKE_SOURCE_DIR}/inc/semantic_analyzer
  ${CMAKE_SOURCE_DIR}/inc/ast
)

add_library(semana_lib_debug 
    ${SOURCES}
)

target_include_directories(semana_lib_debug PUBLIC 
    ${CMAKE_SOURCE_DIR}/inc/semantic_analyzer
    ${CMAKE_SOURCE_DIR}/inc/ast
)

set_target_properties(semana_lib_debug PROPERTIES
    COMPILE_FLAGS "-ggdb3 -O0"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/debug
)
