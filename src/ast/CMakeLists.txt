set(SOURCES
    ASTNode.cpp
    CommandsNode.cpp
    ConditionNode.cpp
    DeclarationsNode.cpp
    ExpressionNode.cpp
    IdentifierNode.cpp
    MainNode.cpp
    ProceduresNode.cpp
    ProgramAllNode.cpp
    ValueNode.cpp
)

add_library(ast_lib
    ${SOURCES}
)

target_include_directories(ast_lib PUBLIC ${CMAKE_SOURCE_DIR}/inc/ast)

add_library(ast_lib_debug
    ${SOURCES}
)

target_include_directories(ast_lib_debug PUBLIC ${CMAKE_SOURCE_DIR}/inc/ast)

set_target_properties(ast_lib_debug PROPERTIES
    COMPILE_FLAGS "-ggdb3 -O0"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/debug
)