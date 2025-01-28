#include "CodeGenerator.hpp"

#include <fstream>
#include <string>

#include "AST.hpp"
#include "ASTNodeFactory.hpp"
#include "Command.hpp"
#include "ErrorMessages.hpp"
#include "InstructNodes.hpp"
#include "Instructions.hpp"

namespace codegen {

CodeGenerator::CodeGenerator()
    : currentProcName(""),
      currentCommand(UNDEFINED),
      lineCounter(0),
      exitCode(semana::SUCCESS),
      accAddr(0) {}

CodeGenerator::~CodeGenerator() {}

semana::ExitCode CodeGenerator::generateCode(compiler::Context &context) {
    this->context = context;
    processNode(context.astRoot);
    saveInstructionsToFile();
    return exitCode;
}

void CodeGenerator::processNode(ASTNode *node) {
    if (!node) throw std::runtime_error("Empty node!");

    switch (node->getNodeType()) {
        case PROGRAM_ALL_NODE: {
            auto programAllNode =
                ast::ASTNodeFactory::castNode<ast::ProgramAllNode>(node);
            for (auto &proc : programAllNode->procedures) {
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
            std::string mainName = "main";
            markers.emplace_back(mainName, ++lineCounter);
            currentProcName = mainName;
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
            currentCommand = ASSIGN;
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
            auto currLineCounter = lineCounter;
            currentCommand = REPEAT;
            processNode(repeatStatementNode->commands);
            processNode(repeatStatementNode->condition);
            auto relativePathDist = currLineCounter - lineCounter;
            // condition result should be put into acc
            addJumpIfAccIsTrue(relativePathDist);

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
            currentCommand = READ;
            processNode(readNode->identifier);
            break;
        }
        case WRITE_NODE: {
            auto writeNode =
                ast::ASTNodeFactory::castNode<ast::WriteNode>(node);
            currentCommand = WRITE;
            processNode(writeNode->value);
            break;
        }
        case PROC_HEAD_NODE: {
            auto procHeadNode =
                ast::ASTNodeFactory::castNode<ast::ProcHeadNode>(node);
            auto procedureName = procHeadNode->pidentifier;
            currentProcName = procedureName;
            markers.emplace_back(procedureName, ++lineCounter);
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
            if (expressionNode->value2.has_value())
            {
                assignNode.waitForThirdArg = true;
                assignNode.operation = static_cast<AssignOperation>(expressionNode->mathOperation.value());
            }
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
            if (valueNode->num.has_value()) {
                auto currentScope = getCurrentScope();
                auto pidentifier = valueNode->num.value();
                auto symbol = context.symbolTable.getSymbolByName(pidentifier,
                                                                  currentScope);
                auto symbolAddress = symbol.address;
                addCommand(pidentifier, symbolAddress);
            }
            break;
        }
        case IDENTIFIER_NODE: {
            auto identifierNode =
                ast::ASTNodeFactory::castNode<ast::IdentifierNode>(node);
            auto currentScope = getCurrentScope();
            if (identifierNode->pidentifier.has_value()) {
                auto pidentifier = identifierNode->pidentifier.value();
                auto symbol = context.symbolTable.getSymbolByName(pidentifier,
                                                                  currentScope);
                auto symbolAddress = symbol.address;
                addCommand(pidentifier, symbolAddress);
            }
            if (identifierNode->Tpidentifier.has_value()) {
                auto pidentifier = identifierNode->Tpidentifier.value();
                auto symbol = context.symbolTable.getSymbolByName(pidentifier,
                                                                  currentScope);
                auto symbolAddress = symbol.address;

                if (identifierNode->arrayNumIndex.has_value()) {
                    auto pidentifier2 = identifierNode->arrayNumIndex.value();
                    auto symbol2 = context.symbolTable.getSymbolByName(
                        pidentifier, currentScope);
                    auto symbolAddress2 = symbol.address;
                }
                if (identifierNode->arrayPidentifierIndex.has_value()) {
                    auto pidentifier2 =
                        identifierNode->arrayPidentifierIndex.value();
                    auto symbol2 = context.symbolTable.getSymbolByName(
                        pidentifier, currentScope);
                    auto symbolAddress2 = symbol.address;
                }
            }
            break;
        }
        default:
            throw std::runtime_error("Unknown node type");
    }
}

int CodeGenerator::getCurrentScope() {
    auto scope = context.symbolTable.getScopeByProcName(currentProcName);
    return scope;
}

void CodeGenerator::addWrite(unsigned long &address) {
    instructions.emplace_back(PUT, address);
    lineCounter++;
}

void CodeGenerator::addRead(unsigned long &address) {
    instructions.emplace_back(GET, address);
    lineCounter++;
}

void CodeGenerator::addAssign(std::vector<Instruction> &instructions) {
    this->instructions.insert(this->instructions.end(), instructions.begin(),
                              instructions.end());
    lineCounter += instructions.size();
}

void CodeGenerator::addJumpIfAccIsTrue(int &jump) {
    instructions.emplace_back(LOAD, accAddr);
    instructions.emplace_back(JPOS, jump);
    lineCounter++;
}

void CodeGenerator::addCommand(std::string &symbolName,
                               unsigned long &address) {
    switch (currentCommand) {
        case READ:{
            addRead(address);
            currentCommand = UNDEFINED;
            break;
        }
        case WRITE: {
            addWrite(address);
            currentCommand = UNDEFINED;
            break;
        }
        case ASSIGN:{
            NodeReadyToGenerateCode res = assignNode.addVariable(address);
            if (res) {
                auto instructions = assignNode.generateCode();
                addAssign(instructions);
                assignNode.clear();
                currentCommand = UNDEFINED;
            }
            break;
        }
        case IF: {
            break;
        }
        case IF_ELSE: {
            break;
        }
        case WHILE: {
            break;
        }
        case REPEAT: {
            break;
        }
        case FOR_TO: {
            break;
        }
        case FORM_FROM: {
            break;
        }
        case PROC_CALL: {
            break;
        }
        case UNDEFINED: {
            /*throw std::runtime_error("Undefined command");*/
            break;
        }
    }

}

void CodeGenerator::saveInstructionsToFile() {
    std::ofstream outFile(context.outputFile);
    if (!outFile.is_open()) {
        throw std::runtime_error("Unable to open " + context.outputFile +
                                 " for writing.");
    }

    for (const auto &i : instructions) {
        outFile << i.opcode << " " << i.value << std::endl;
    }

    outFile.close();
}

}  // namespace codegen
