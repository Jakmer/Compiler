#include <stdexcept>

#include "ASTNodeFactory.hpp"
#include "SemanticAnalzyer.hpp"

namespace semana {

SemanticAnalyzer::SemanticAnalyzer(std::string& filename) {
    this->filename = filename;
    this->astRoot = nullptr;
}

SemanticAnalyzer::~SemanticAnalyzer() {}

void SemanticAnalyzer::analyze(ASTNode* root) { 
    processNode(root);
}

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

    std::string globalScopeName = "global";

    unsigned int procCounter = 0;

    switch (node->getNodeType()) {
        case PROGRAM_ALL_NODE: {
            symbolTable.openScope(globalScopeName, ScopeType::GLOBAL);
            auto programAllNode =
                ast::ASTNodeFactory::castNode<ast::ProgramAllNode>(node);
            for (auto& proc : programAllNode->procedures) {
                processNode(proc);
            }
            processNode(programAllNode->main);
            symbolTable.closeScope();
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
            std::string mainScopeName = "main";
            symbolTable.openScope(mainScopeName, ScopeType::MAIN);
            auto mainNode = ast::ASTNodeFactory::castNode<ast::MainNode>(node);
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
            // pidentfier cant be changed in for loop
            Symbol symbol(semana::VARIABLE_NAME,
                          semana::VariableType::FOR_ITERATOR, pidentifier);
            auto validationMessage = symbolTable.insert(symbol);
            if (validationMessage.errorType != ErrorType::GOOD) {
                auto location = getLocation(node);
                raiseError(validationMessage, location);
            }
            processNode(forToNode->valueFrom);
            processNode(forToNode->valueTo);
            processNode(forToNode->commands);
            break;
        }
        case FOR_DOWNTO_NODE: {
            auto forDowntoNode =
                ast::ASTNodeFactory::castNode<ast::ForDowntoNode>(node);
            std::string pidentifier = forDowntoNode->pidentifier;
            // pidentfier cant be changed in for loop
            Symbol symbol(semana::VARIABLE_NAME,
                          semana::VariableType::FOR_ITERATOR, pidentifier);
            auto validationMessage = symbolTable.insert(symbol);
            if (validationMessage.errorType != ErrorType::GOOD) {
                auto location = getLocation(node);
                raiseError(validationMessage, location);
            }
            processNode(forDowntoNode->valueFrom);
            processNode(forDowntoNode->valueTo);
            processNode(forDowntoNode->commands);
            break;
        }
        case PROC_CALL_NODE: {
            auto procCallNode =
                ast::ASTNodeFactory::castNode<ast::ProcCallNode>(node);
            std::string procName = procCallNode->pidentifier;
            Symbol procSymbol(semana::PROCEDURE_NAME,
                              semana::VariableType::PROCEDURE, procName);
            auto validationMessage = symbolTable.validateSymbol(procSymbol);
            if (validationMessage.errorType != ErrorType::GOOD) {
                auto location = getLocation(node);
                raiseError(validationMessage, location);
            }
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
            Symbol procSymbol(semana::PROCEDURE_NAME,
                              semana::VariableType::PROCEDURE, procName);
            auto validationMessage = symbolTable.insert(procSymbol);
            if (validationMessage.errorType != ErrorType::GOOD) {
                auto location = getLocation(node);
                raiseError(validationMessage, location);
            }
            symbolTable.openScope(procName, ScopeType::PROCEDURE);
            processNode(procHeadNode->args_decl);
            break;
        }
        case DECLARATIONS_NODE: {
            auto declarationsNode =
                ast::ASTNodeFactory::castNode<ast::DeclarationsNode>(node);
            for (auto& pidentifier : declarationsNode->pidentifiers) {
                Symbol varSymbol(semana::VARIABLE_NAME,
                                 semana::VariableType::NUM, pidentifier);
                auto validationMessage = symbolTable.insert(varSymbol);
                if (validationMessage.errorType != ErrorType::GOOD) {
                    auto location = getLocation(node);
                    raiseError(validationMessage, location);
                }
            }
            for (auto& Tpidentifier : declarationsNode->arrays) {
                ast::array array = Tpidentifier;
                Symbol arraySymbol(semana::ARRAY_NAME,
                                   semana::VariableType::ARRAY, array);
                auto validationMessage = symbolTable.insert(arraySymbol);
                if (validationMessage.errorType != ErrorType::GOOD) {
                    auto location = getLocation(node);
                    raiseError(validationMessage, location);
                }
            }
            break;
        }
        case ARGS_DECL_NODE: {
            auto argsDeclNode =
                ast::ASTNodeFactory::castNode<ast::ArgsDeclNode>(node);
            for (auto& arg : argsDeclNode->pidentifiers) {
                Symbol argSymbol(semana::VARIABLE_NAME,
                                 semana::VariableType::NUM, arg);
                auto validationMessage = symbolTable.insert(argSymbol);
                if (validationMessage.errorType != ErrorType::GOOD) {
                    auto location = getLocation(node);
                    raiseError(validationMessage, location);
                }
            }
            for (auto& arg : argsDeclNode->Tpidentifiers) {
                ast::array argArray = {arg, 0, 0};
                Symbol argSymbol(semana::ARRAY_NAME,
                                 semana::VariableType::ARRAY, arg);
                auto validationMessage = symbolTable.insert(argSymbol);
                if (validationMessage.errorType != ErrorType::GOOD) {
                    auto location = getLocation(node);
                    raiseError(validationMessage, location);
                }
            }
            break;
        }
        case ARGS_NODE: {
            auto argsNode = ast::ASTNodeFactory::castNode<ast::ArgsNode>(node);
            for (auto& arg : argsNode->pidentifiers) {
                Symbol argSymbol(semana::VARIABLE_NAME,
                                 semana::VariableType::NUM, arg);
                auto validationMessage = symbolTable.validateSymbol(argSymbol);
                // validate wheter the type of the argument is correct
                if (validationMessage.errorType == ErrorType::ITERATOR) {
                    validationMessage = validateIterator(argsNode);
                }
                if (validationMessage.errorType != ErrorType::GOOD) {
                    auto location = getLocation(node);
                    raiseError(validationMessage, location);
                }
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
            break;
        }
        case IDENTIFIER_NODE: {
            auto identifierNode =
                ast::ASTNodeFactory::castNode<ast::IdentifierNode>(node);
            // Process identifierNode if needed
            if (identifierNode->pidentifier.has_value()) {
                Symbol symbol(semana::VARIABLE_NAME, semana::VariableType::NUM,
                              identifierNode->pidentifier.value());
                auto validationMessage = symbolTable.validateSymbol(symbol);
                if (validationMessage.errorType == ErrorType::ITERATOR) {
                    validationMessage = validateIterator(identifierNode);
                }
                if (validationMessage.errorType != ErrorType::GOOD) {
                    auto location = getLocation(node);
                    raiseError(validationMessage, location);
                }
            } else if (identifierNode->Tpidentifier.has_value()) {
                std::string pidentifier = identifierNode->Tpidentifier.value();
                if (identifierNode->arrayNumIndex.has_value()) {
                    // TODO: check if arrayIndex is in range
                    int arrayIndex = identifierNode->arrayNumIndex.value();

                    // here I use 0,0 as a placeholder for array range because I
                    // don't have the information about the array range
                    Symbol symbol(semana::ARRAY_NAME,
                                  semana::VariableType::ARRAY,
                                  ast::array{pidentifier, 0, 0});
                    auto validationMessage = symbolTable.validateSymbol(symbol);
                    if (validationMessage.errorType != ErrorType::GOOD) {
                        auto location = getLocation(node);
                        raiseError(validationMessage, location);
                    }
                } else if (identifierNode->arrayPidentifierIndex.has_value()) {
                    std::string arrayIndex =
                        identifierNode->arrayPidentifierIndex.value();
                    Symbol symbol(semana::VARIABLE_NAME,
                                  semana::VariableType::NUM, arrayIndex);
                    auto validationMessage = symbolTable.validateSymbol(symbol);
                    if (validationMessage.errorType == ErrorType::ITERATOR) {
                        validationMessage = validateIterator(identifierNode);
                    }
                    if (validationMessage.errorType != ErrorType::GOOD) {
                        auto location = getLocation(node);
                        raiseError(validationMessage, location);
                    }

                    Symbol symbolArray(semana::ARRAY_NAME,
                                       semana::VariableType::ARRAY,
                                       ast::array{pidentifier, 0, 0});
                    auto validationMessageArray =
                        symbolTable.validateSymbol(symbolArray);

                    if (validationMessageArray.errorType != ErrorType::GOOD) {
                        auto location = getLocation(node);
                        raiseError(validationMessageArray, location);
                    }
                }
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
        case ErrorType::ITERATOR: {
            std::cerr << redColorCode << "Iterator: " << location
                      << message.content << resetColorCode << std::endl;
            break;
        }
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

    // store FOR_TO_NODE or FOR_DOWNTO_NODE node in stack and check if iterator is used in loop scope

    return {ErrorType::GOOD, "Good"};
}

}  // namespace semana
