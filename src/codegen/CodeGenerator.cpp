#include "CodeGenerator.hpp"
#include "ASTNodeFactory.hpp"
#include "AST.hpp"
#include "ErrorMessages.hpp"

namespace codegen {

semana::ExitCode CodeGenerator::generateCode(compiler::Context &context) {
    exitCode = semana::SUCCESS;
    processNode(context.astRoot);
    return exitCode;
}

void CodeGenerator::processNode(ASTNode *node) {
    if (!node) throw std::runtime_error("Empty node!");

    switch (node->getNodeType()) {
        case PROGRAM_ALL_NODE: {
            auto programAllNode =
                ast::ASTNodeFactory::castNode<ast::ProgramAllNode>(node);
            for (auto& proc : programAllNode->procedures) {
                processNode(proc);
            }
            processNode(programAllNode->main);
            break;
        }
        case PROCEDURES_NODE: {
            auto proceduresNode =
                ast::ASTNodeFactory::castNode<ast::ProceduresNode>(node);
            processNode(proceduresNode->proc_head);
            if (proceduresNode->declarations.has_value()) {
                processNode(proceduresNode->declarations.value());
            }
            processNode(proceduresNode->commands);
            break;
        }
        case MAIN_NODE: {
            auto mainNode = ast::ASTNodeFactory::castNode<ast::MainNode>(node);
            processNode(mainNode->declarations);
            processNode(mainNode->commands);
            break;
        }
        case COMMANDS_NODE: {
            auto commandsNode =
                ast::ASTNodeFactory::castNode<ast::CommandsNode>(node);
            for (auto &cmd : commandsNode->commands) {
                processNode(cmd);
            }
            break;
        }
        case ASSIGNMENT_NODE: {
            auto assignmentNode =
                ast::ASTNodeFactory::castNode<ast::AssignmentNode>(node);
            processNode(assignmentNode->identifier);
            processNode(assignmentNode->expression);
            break;
        }
        case IF_STATEMENT_NODE: {
            auto ifStatementNode =
                ast::ASTNodeFactory::castNode<ast::IfStatementNode>(node);
            processNode(ifStatementNode->condition);
            processNode(ifStatementNode->commands);
            if (ifStatementNode->elseCommands.has_value()) {
                processNode(ifStatementNode->elseCommands.value());
            }
            break;
        }
        case WHILE_STATEMENT_NODE: {
            auto whileStatementNode =
                ast::ASTNodeFactory::castNode<ast::WhileStatementNode>(node);
            processNode(whileStatementNode->condition);
            processNode(whileStatementNode->commands);
            break;
        }
        case REPEAT_STATEMENT_NODE: {
            auto repeatStatementNode =
                ast::ASTNodeFactory::castNode<ast::RepeatStatementNode>(node);
            processNode(repeatStatementNode->condition);
            processNode(repeatStatementNode->commands);
            break;
        }
        case FOR_TO_NODE: {
            auto forToNode =
                ast::ASTNodeFactory::castNode<ast::ForToNode>(node);
            processNode(forToNode->valueFrom);
            processNode(forToNode->valueTo);
            processNode(forToNode->commands);
            break;
        }
        case FOR_DOWNTO_NODE: {
            auto forDowntoNode =
                ast::ASTNodeFactory::castNode<ast::ForDowntoNode>(node);
            processNode(forDowntoNode->valueFrom);
            processNode(forDowntoNode->valueTo);
            processNode(forDowntoNode->commands);
            break;
        }
        case PROC_CALL_NODE: {
            auto procCallNode =
                ast::ASTNodeFactory::castNode<ast::ProcCallNode>(node);
            processNode(procCallNode->args);
            break;
        }
        case READ_NODE: {
            auto readNode = ast::ASTNodeFactory::castNode<ast::ReadNode>(node);
            processNode(readNode->identifier);
            break;
        }
        case WRITE_NODE: {
            auto writeNode =
                ast::ASTNodeFactory::castNode<ast::WriteNode>(node);
            processNode(writeNode->value);
            break;
        }
        case PROC_HEAD_NODE: {
            auto procHeadNode =
                ast::ASTNodeFactory::castNode<ast::ProcHeadNode>(node);
            processNode(procHeadNode->args_decl);
            break;
        }
        case DECLARATIONS_NODE: {
            auto declarationsNode =
                ast::ASTNodeFactory::castNode<ast::DeclarationsNode>(node);
            break;
        }
        case ARGS_DECL_NODE: {
            auto argsDeclNode =
                ast::ASTNodeFactory::castNode<ast::ArgsDeclNode>(node);
            break;
        }
        case ARGS_NODE: {
            auto argsNode = ast::ASTNodeFactory::castNode<ast::ArgsNode>(node);
            break;
        }
        case EXPRESSION_NODE: {
            auto expressionNode =
                ast::ASTNodeFactory::castNode<ast::ExpressionNode>(node);
            processNode(expressionNode->value1);
            if (expressionNode->value2.has_value()) {
                processNode(expressionNode->value2.value());
            }
            break;
        }
        case CONDITION_NODE: {
            auto conditionNode =
                ast::ASTNodeFactory::castNode<ast::ConditionNode>(node);
            processNode(conditionNode->value1);
            processNode(conditionNode->value2);
            break;
        }
        case VALUE_NODE: {
            auto valueNode =
                ast::ASTNodeFactory::castNode<ast::ValueNode>(node);
            if (valueNode->identifier.has_value()) {
                processNode(valueNode->identifier.value());
            }
            break;
        }
        case IDENTIFIER_NODE: {
            auto identifierNode =
                ast::ASTNodeFactory::castNode<ast::IdentifierNode>(node);
            break;
        }
        default:
            throw std::runtime_error("Unknown node type");
    }
}

}  // codegen
