add_library(ast_lib
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

target_include_directories(ast_lib PUBLIC ${CMAKE_SOURCE_DIR}/inc/ast)
