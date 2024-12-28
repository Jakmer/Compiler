#include "ASTNodeFactory.hpp"

namespace ast {

ASTNode* ASTNodeFactory::createNode(NodeType type) {
    switch (type) {
        case COMMANDS_NODE:
            return new CommandsNode();
        case ASSIGNMENT_NODE:
            return new AssignmentNode();
        case IF_STATEMENT_NODE:
            return new IfStatementNode();
        case WHILE_STATEMENT_NODE:
            return new WhileStatementNode();
        case REPEAT_STATEMENT_NODE:
            return new RepeatStatementNode();
        case FOR_TO_NODE:
            return new ForToNode();
        case FOR_DOWNTO_NODE:
            return new ForDowntoNode();
        case PROC_CALL_NODE:
            return new ProcCallNode();
        case ARGS_NODE:
            return new ArgsNode();
        case READ_NODE:
            return new ReadNode();
        case WRITE_NODE:
            return new WriteNode();
        case EXPRESSION_NODE:
            return new ExpressionNode();
        case IDENTIFIER_NODE:
            return new IdentifierNode();
        case DECLARATIONS_NODE:
            return new DeclarationsNode();
        case PROGRAM_ALL_NODE:
            return new ProgramAllNode();
        case MAIN_NODE:
            return new MainNode();
        case VALUE_NODE:
            return new ValueNode();
        case CONDITION_NODE:
            return new ConditionNode();
        case PROCEDURES_NODE:
            return new ProceduresNode();
        case PROC_HEAD_NODE:
            return new ProcHeadNode();
        case ARGS_DECL_NODE:
            return new ArgsDeclNode();
        default:
            throw std::runtime_error("Unknown node type");
            return nullptr;
    }
}

}  // namespace ast
