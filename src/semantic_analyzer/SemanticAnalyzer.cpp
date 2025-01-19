#include <stdexcept>

#include "ASTNodeFactory.hpp"
#include "SemanticAnalzyer.hpp"

namespace semana {

SemanticAnalyzer::SemanticAnalyzer(std::string& filename) {
    this->filename = filename;
    this->astRoot = nullptr;
}

SemanticAnalyzer::~SemanticAnalyzer() {}

void SemanticAnalyzer::analyze(ASTNode* root) { processNode(root); }

bool SemanticAnalyzer::hasErrors() const { return false; }

const std::vector<std::string>& SemanticAnalyzer::getErrors() const {
    return errors;
}

void SemanticAnalyzer::checkVariableDeclarations(
    const std::vector<std::string>& tokens) {}

void SemanticAnalyzer::checkFunctionDefinitions(
    const std::vector<std::string>& tokens) {}

void SemanticAnalyzer::processNode(ASTNode* node) {
    if (!node) throw std::runtime_error("Empty node!");

    switch (node->getNodeType()) {
        case PROGRAM_ALL_NODE: {
            auto programAllNode =
                ast::ASTNodeFactory::castNode<ast::ProgramAllNode>(node);
            symbolTable.openScope(ScopeType::GLOBAL_SCOPE); // we can ignore returned message here bc if something goes wrong it will return exception not message at this point
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
            symbolTable.closeScope();
            break;
        }
        case MAIN_NODE: {
            auto mainNode = ast::ASTNodeFactory::castNode<ast::MainNode>(node);
            symbolTable.openScope(MAIN_SCOPE);
            processNode(mainNode->declarations);
            processNode(mainNode->commands);
            symbolTable.closeScope();
            break;
        }
        case COMMANDS_NODE: {
            auto commandsNode =
                ast::ASTNodeFactory::castNode<ast::CommandsNode>(node);
            for (auto& cmd : commandsNode->commands) {
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
            std::string pidentifier = forToNode->pidentifier;
            processNode(forToNode->valueFrom);
            processNode(forToNode->valueTo);
            processNode(forToNode->commands);
            break;
        }
        case FOR_DOWNTO_NODE: {
            auto forDowntoNode =
                ast::ASTNodeFactory::castNode<ast::ForDowntoNode>(node);
            std::string pidentifier = forDowntoNode->pidentifier;
            processNode(forDowntoNode->valueFrom);
            processNode(forDowntoNode->valueTo);
            processNode(forDowntoNode->commands);
            break;
        }
        case PROC_CALL_NODE: {
            auto procCallNode =
                ast::ASTNodeFactory::castNode<ast::ProcCallNode>(node);
            std::string procName = procCallNode->pidentifier;
            Symbol symbol(procName, PROCEDURE);
            // validate here
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
            auto procName = procHeadNode->pidentifier;
            Symbol symbol(procName, PROCEDURE);
            symbolTable.openScope(PROCEDURE_SCOPE);
            symbolTable.addSymbol(symbol);
            processNode(procHeadNode->args_decl);
            break;
        }
        case DECLARATIONS_NODE: {
            auto declarationsNode =
                ast::ASTNodeFactory::castNode<ast::DeclarationsNode>(node);
            for(auto &array : declarationsNode->arrays)
            {
                Symbol symbol(array.pidentifier, ARRAY, array);
                symbolTable.addSymbol(symbol);
            }
            for(auto &pidentifier : declarationsNode->pidentifiers)
            {
                Symbol symbol(pidentifier, VARIABLE);
                symbolTable.addSymbol(symbol);
            }
            break;
        }
        case ARGS_DECL_NODE: {
            auto argsDeclNode =
                ast::ASTNodeFactory::castNode<ast::ArgsDeclNode>(node);
            for(auto &pidentifer : argsDeclNode->pidentifiers)
            {
                Symbol symbol(pidentifer, VARIABLE);
                symbolTable.addSymbol(symbol);
            }
            for(auto &Tpidentifier : argsDeclNode->Tpidentifiers)
            {
                Symbol symbol(Tpidentifier, ARRAY);
                symbolTable.addSymbol(symbol);
            }
            break;
        }
        case ARGS_NODE: {
            auto argsNode = ast::ASTNodeFactory::castNode<ast::ArgsNode>(node);
            for(auto &pidentifer : argsNode->pidentifiers)
            {
                Symbol symbol(pidentifer, VARIABLE);
                symbolTable.addSymbol(symbol);
            }
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
            if(valueNode->num.has_value())
            {
                Symbol symbol(valueNode->num.value(), RVALUE);
                /*symbolTable.addSymbol(symbol);*/
                // validate here
            }
            break;
        }
        case IDENTIFIER_NODE: {
            auto identifierNode =
                ast::ASTNodeFactory::castNode<ast::IdentifierNode>(node);
            if(identifierNode->pidentifier.has_value())
            {
                Symbol symbol(identifierNode->pidentifier.value(), VARIABLE);
                /*symbolTable.addSymbol(symbol);*/
                // validate here
            }
            if(identifierNode->Tpidentifier.has_value())
            {
                Symbol symbol(identifierNode->Tpidentifier.value(), ARRAY);
                /*symbolTable.addSymbol(symbol);*/
                // validate here
            }
            if(identifierNode->arrayNumIndex.has_value())
            {
                Symbol symbol(identifierNode->arrayNumIndex.value(), RVALUE);
                /*symbolTable.addSymbol(symbol);*/
                // validate here
            }
            if(identifierNode->arrayPidentifierIndex.has_value())
            {
                Symbol symbol(identifierNode->arrayPidentifierIndex.value(), VARIABLE);
                /*symbolTable.addSymbol(symbol);*/
                // validate here
            }
            break;
        }
        default:
            throw std::runtime_error("Unknown node type");
    }
}

void SemanticAnalyzer::raiseError(ValidationMessage& message,
                                  std::string& location) {
    const std::string redColorCode = "\033[31m";
    const std::string resetColorCode = "\033[0m";

    switch (message.errorType) {
        case ErrorType::UNDECLARED_VARIABLE: {
            std::cerr << redColorCode << "Undeclared variable: " << location
                      << message.content << resetColorCode << std::endl;
            break;
        }
        case ErrorType::INCORRECT_ARRAY_RANGE: {
            std::cerr << redColorCode << "Incorrect array range: " << location
                      << message.content << resetColorCode << std::endl;
            break;
        }
        case ErrorType::DUPLICATE_DECLARATION: {
            std::cerr << redColorCode << "Duplicate declaration: " << location
                      << message.content << resetColorCode << std::endl;
            break;
        }
        case ErrorType::UNDECLARED_PROCEDURE: {
            std::cerr << redColorCode << "Undeclared procedure: " << location
                      << message.content << resetColorCode << std::endl;
            break;
        }
        case ErrorType::INCORRECT_ARGUMENTS: {
            std::cerr << redColorCode << "Incorrect arguments: " << location
                      << message.content << resetColorCode << std::endl;
            break;
        }
        case ErrorType::UNDECLARED_ARRAY: {
            std::cerr << redColorCode << "Undeclared array: " << location
                      << message.content << resetColorCode << std::endl;
            break;
        }
        case ErrorType::UNDECLARED_MAIN: {
            std::cerr << redColorCode << "Undeclared main: " << location
                      << message.content << resetColorCode << std::endl;
            break;
        }
        case ErrorType::UNDECLARED_GLOBAL: {
            std::cerr << redColorCode << "Undeclared global: " << location
                      << message.content << resetColorCode << std::endl;
            break;
        }
        case ErrorType::TYPE_MISMATCH: {
            std::cerr << redColorCode << "Type mismatch: " << location
                      << message.content << resetColorCode << std::endl;
            break;
        }
        case ErrorType::RECURSIVE_CALL: {
            std::cerr << redColorCode << "Recursive call: " << location
                      << message.content << resetColorCode << std::endl;
            break;
        }
        case ErrorType::UNINITIALIZED_VARIABLE: {
            std::cerr << redColorCode << "Uninitialized variable: " << location
                      << message.content << resetColorCode << std::endl;
            break;
        }
        case ErrorType::UNINITIALIZED_ARRAY: {
            std::cerr << redColorCode << "Uninitialized array: " << location
                      << message.content << resetColorCode << std::endl;
            break;
        }
        /*case ErrorType::ITERATOR: {*/
        /*    std::cerr << redColorCode << "Iterator: " << location*/
        /*              << message.content << resetColorCode << std::endl;*/
        /*    break;*/
        /*}*/
        default:
            throw std::runtime_error("Unknown error type");
    }
}

std::string SemanticAnalyzer::getLocation(ASTNode* node) {
    auto pos = node->getPosition();
    return filename + ":" + std::to_string(pos.line) + ":" +
           std::to_string(pos.column) + ": ";
}

void SemanticAnalyzer::printSymbolTable() { symbolTable.printSymbols(); }

ValidationMessage SemanticAnalyzer::validateIterator(ASTNode* node) {
    // TODO: implement function that checks if we dont modify iterator and if
    // iterator is used only in loop scope NODE
    auto isForLoop = [](ASTNode* node) {
        return node->getNodeType() == FOR_TO_NODE ||
               node->getNodeType() == FOR_DOWNTO_NODE;
    };

    // store FOR_TO_NODE or FOR_DOWNTO_NODE node in stack and check if iterator
    // is used in loop scope

    return {ErrorType::GOOD, "Good"};
}

}  // namespace semana
