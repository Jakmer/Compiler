#include "CodeGenerator.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include "ASTNode.hpp"
#include "ASTNodeFactory.hpp"
#include "Command.hpp"
#include "Context.hpp"
#include "ErrorMessages.hpp"
#include "InstructNodes.hpp"
#include "Instructions.hpp"
#include "Memory.hpp"
#include "ProceduresNode.hpp"
#include "SymbolTable.hpp"

namespace codegen {

CodeGenerator::CodeGenerator(compiler::Context &context)
    : currentProcName(""),
      currentCommand(UNDEFINED),
      lineCounter(0),
      exitCode(semana::SUCCESS),
      noConditions(0),
      noRepeats(0),
      noWhiles(0),
      noFors(0),
      accAddr(0),
      context(context),
      memory(context.symbolTable.getLastUsedAddr()),
      assignNode(memory),
      conditionNode(memory),
      repeatNode(memory),
      whileNode(memory),
      forNode(memory) {}

CodeGenerator::~CodeGenerator() {}

semana::ExitCode CodeGenerator::generateCode() {
    setRValues();
    jumpToMain();
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
            addHalt();
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

            // jump back to main
            auto procHeadNode =
                ast::ASTNodeFactory::castNode<ast::ProcHeadNode>(
                    proceduresNode->proc_head);
            auto procName = procHeadNode->pidentifier;
            auto returnReg = context.symbolTable.getProcedureAddr(procName);
            instructions.emplace_back(RTRN, returnReg);
            lineCounter++;
            break;
        }
        case MAIN_NODE: {
            auto mainNode = ast::ASTNodeFactory::castNode<ast::MainNode>(node);
            std::string mainName = "main";
            auto it = std::find_if(markers.begin(), markers.end(),
                                   [&mainName](const Marker &marker) {
                                       return marker.name == mainName;
                                   });
            it->line += lineCounter;
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
            // label condition
            noConditions++;
            currentCommand = CONDITION;
            std::string label = "condition" + std::to_string(noConditions);
            this->conditionNode.name = label;
            processNode(ifStatementNode->condition);
            auto currLineCounter1 = lineCounter;
            processNode(ifStatementNode->commands);
            auto currLineCounter2 = lineCounter;
            auto relativePathDist = currLineCounter2 - currLineCounter1 + 1;
            if (ifStatementNode->elseCommands.has_value()) relativePathDist++;
            markers.emplace_back(label, relativePathDist);
            // find jump to label in instruction and change it to relative path
            if (ifStatementNode->elseCommands.has_value()) {
                std::string label =
                    "condition_else" + std::to_string(noConditions);
                this->conditionNode.name = label;
                this->conditionNode.elseExist = true;
                instructions.emplace_back(JUMP, label);
                lineCounter++;
                auto currLineCounter1 = lineCounter;
                processNode(ifStatementNode->elseCommands.value());
                auto currLineCounter2 = lineCounter;
                auto relativePathDist = currLineCounter2 - currLineCounter1 + 1;
                markers.emplace_back(label, relativePathDist);
            }
            break;
        }
        case WHILE_STATEMENT_NODE: {
            auto whileStatementNode =
                ast::ASTNodeFactory::castNode<ast::WhileStatementNode>(node);
            currentCommand = WHILE;
            noWhiles++;
            std::string label1 = "while_cond" + std::to_string(noWhiles);
            std::string label2 = "while_beg" + std::to_string(noWhiles);
            this->whileNode.name = label1;
            auto currLineCounter1 = lineCounter;
            processNode(whileStatementNode->condition);
            auto currLineCounter2 = lineCounter;
            processNode(whileStatementNode->commands);
            auto currLineCounter3 = lineCounter;
            auto relativePathDist1 = currLineCounter3 - currLineCounter2 + 2;
            auto relativePathDist2 = currLineCounter1 - currLineCounter3;
            markers.emplace_back(label1, relativePathDist1);
            markers.emplace_back(label2, relativePathDist2);
            instructions.emplace_back(JUMP, label2);
            lineCounter++;
            break;
        }
        case REPEAT_STATEMENT_NODE: {
            auto repeatStatementNode =
                ast::ASTNodeFactory::castNode<ast::RepeatStatementNode>(node);
            auto currLineCounter1 = lineCounter;
            noRepeats++;
            std::string label = "repeat" + std::to_string(noRepeats);
            processNode(repeatStatementNode->commands);
            currentCommand = REPEAT;
            this->repeatNode.name = label;
            processNode(repeatStatementNode->condition);
            auto currLineCounter2 = lineCounter;
            auto relativePathDist = currLineCounter1 - currLineCounter2 + 1;
            markers.emplace_back(label, relativePathDist);
            break;
        }
        case FOR_TO_NODE: {
            auto forToNode =
                ast::ASTNodeFactory::castNode<ast::ForToNode>(node);
            auto currentScope = getCurrentScope();
            auto pidentifier = forToNode->pidentifier;
            auto symbol =
                context.symbolTable.getSymbolByName(pidentifier, currentScope);
            auto symbolAddress = symbol.address;
            noFors++;
            currentCommand = FOR_TO;
            this->forNode.iterator = symbolAddress;
            this->forNode.mode = UP_STEP;
            auto currLineCounter1 = lineCounter;
            std::string label1 = "beg_for_to" + std::to_string(noFors);
            std::string label2 = "end_for_to" + std::to_string(noFors);
            this->forNode.name = label2;
            processNode(forToNode->valueFrom);
            processNode(forToNode->valueTo);
            processNode(forToNode->commands);
            // increment iterator
            instructions.emplace_back(
                SET, 1);  // TODO: change it to reusing "1" rvalue instead of
                          // setting it in each iteration
            instructions.emplace_back(ADD, symbolAddress);
            instructions.emplace_back(STORE, symbolAddress);
            instructions.emplace_back(JUMP, label1);
            lineCounter += 4;
            auto currLineCounter2 = lineCounter;
            auto relativePathDist1 = currLineCounter1 - currLineCounter2 + 3;
            auto relativePathDist2 = currLineCounter2 - currLineCounter1 - 4;
            markers.emplace_back(label1, relativePathDist1);
            markers.emplace_back(label2, relativePathDist2);
            break;
        }
        case FOR_DOWNTO_NODE: {
            auto forDowntoNode =
                ast::ASTNodeFactory::castNode<ast::ForDowntoNode>(node);
            auto currentScope = getCurrentScope();
            auto pidentifier = forDowntoNode->pidentifier;
            auto symbol =
                context.symbolTable.getSymbolByName(pidentifier, currentScope);
            auto symbolAddress = symbol.address;
            noFors++;
            currentCommand = FOR_DOWNTO;
            this->forNode.iterator = symbolAddress;
            this->forNode.mode = DOWN_STEP;
            std::string label1 = "beg_for_down_to" + std::to_string(noFors);
            std::string label2 = "end_for_down_to" + std::to_string(noFors);
            this->forNode.name = label2;
            processNode(forDowntoNode->valueFrom);
            processNode(forDowntoNode->valueTo);
            auto currLineCounter1 = lineCounter;
            processNode(forDowntoNode->commands);
            // increment iterator
            instructions.emplace_back(
                SET, 1);  // TODO: change it to reusing "1" rvalue instead of
                          // setting it in each iteration
            auto freeReg = memory.getFreeRegister();
            memory.lockReg(freeReg);
            instructions.emplace_back(STORE, freeReg);
            instructions.emplace_back(LOAD, symbolAddress);
            instructions.emplace_back(SUB, freeReg);
            instructions.emplace_back(STORE, symbolAddress);
            instructions.emplace_back(JUMP, label1);
            lineCounter += 6;
            auto currLineCounter2 = lineCounter;
            auto relativePathDist1 = currLineCounter1 - currLineCounter2 - 2;
            auto relativePathDist2 = currLineCounter2 - currLineCounter1 + 1;
            markers.emplace_back(label1, relativePathDist1);
            markers.emplace_back(label2, relativePathDist2);
            break;
        }
        case PROC_CALL_NODE: {
            auto procCallNode =
                ast::ASTNodeFactory::castNode<ast::ProcCallNode>(node);
            auto procName = procCallNode->pidentifier;
            currProcCallName = procName;
            processNode(procCallNode->args);

            // prepare return register
            auto procAddr = context.symbolTable.getProcedureAddr(procName);
            instructions.emplace_back(SET, lineCounter + 3);
            instructions.emplace_back(STORE, procAddr);
            lineCounter += 2;

            // jump to procedure
            auto currLine = lineCounter;
            auto it = std::find_if(markers.begin(), markers.end(),
                                   [&procName](const Marker &marker) {
                                       return marker.name == procName;
                                   });
            auto jump = it->line - currLine;
            instructions.emplace_back(JUMP, jump);
            lineCounter++;
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
            markers.emplace_back(procedureName, lineCounter);
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

            // set pointers in procedure
            int noArgs = 0;
            for (auto &arg : argsNode->pidentifiers) {
                noArgs++;
                auto currentScope = getCurrentScope();
                auto argSym =
                    context.symbolTable.getSymbolByName(arg, currentScope);
                auto argAddr = argSym.address;

                auto procScope =
                    context.symbolTable.getScopeByProcName(currProcCallName);
                auto ptrAddr = context.symbolTable.getAddrOfProcArg(
                    currProcCallName, noArgs);

                heap.push_back(ptrAddr);

                // if we call from another function (not main) then LOAD adres
                // instead of setting bc argAddr will be already ptr type

                if (currentProcName == "main" ||
                    (currentProcName != "main" &&
                     !isProcArgument(argSym.name, currentScope))) {
                    instructions.emplace_back(SET, argAddr);
                } else {
                    instructions.emplace_back(LOAD, argAddr, true);
                }
                instructions.emplace_back(STORE, ptrAddr, true);
                lineCounter += 2;
            }
            break;
        }
        case EXPRESSION_NODE: {
            auto expressionNode =
                ast::ASTNodeFactory::castNode<ast::ExpressionNode>(node);
            if (expressionNode->value2.has_value()) {
                assignNode.waitForThirdArg = true;
                assignNode.operation = static_cast<AssignOperation>(
                    expressionNode->mathOperation
                        .value());  // be careful here - enums may be not
                                    // mapped in exactly same way
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
            this->conditionNode.operation = static_cast<ConditionOperation>(
                conditionNode->relation);  // be careful here - enums may be
                                           // not mapped in exactly same way
            if (currentCommand == REPEAT) {
                this->repeatNode.operation =
                    static_cast<ConditionOperation>(conditionNode->relation);
            }
            if (currentCommand == WHILE) {
                this->whileNode.operation =
                    static_cast<ConditionOperation>(conditionNode->relation);
            }
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
                if (isProcArgument(pidentifier, currentScope))
                    heap.push_back(symbolAddress);
                addCommand(pidentifier, symbolAddress);
            }
            if (identifierNode->Tpidentifier.has_value()) {
                auto pidentifier = identifierNode->Tpidentifier.value();
                auto symbol = context.symbolTable.getSymbolByName(pidentifier,
                                                                  currentScope);
                auto symbolAddress = symbol.address;
                if (isProcArgument(pidentifier, currentScope)) {
                    auto pointer =
                        context.symbolTable
                            .getSymbolByName(pidentifier, currentScope)
                            .address;

                    auto newPointer = memory.getFreeRegister();
                    memory.lockReg(newPointer);  // needs to be unlocked
                    heap.push_back(newPointer);

                    if (identifierNode->arrayNumIndex.has_value()) {
                        auto pidentifier2 =
                            identifierNode->arrayNumIndex.value();
                        auto symbol2 = context.symbolTable.getSymbolByName(
                            pidentifier2, currentScope);
                        auto symbolAddress2 = symbol2.address;
                        instructions.emplace_back(LOAD, pointer, true);
                        instructions.emplace_back(ADD, symbolAddress2);
                        instructions.emplace_back(STORE, newPointer, true);

                        lineCounter += 3;
                        addCommand(pidentifier, newPointer);
                    }
                    if (identifierNode->arrayPidentifierIndex.has_value()) {
                        auto pidentifier2 =
                            identifierNode->arrayPidentifierIndex.value();
                        auto symbol2 = context.symbolTable.getSymbolByName(
                            pidentifier2, currentScope);
                        auto symbolAddress2 = symbol2.address;
                        instructions.emplace_back(LOAD, pointer, true);
                        instructions.emplace_back(ADD, symbolAddress2);
                        instructions.emplace_back(STORE, newPointer, true);
                        lineCounter += 3;
                        addCommand(pidentifier, newPointer);
                    }
                } else {
                    auto pointer = memory.getFreeRegister();
                    memory.lockReg(pointer);  // needs to be unlocked
                    heap.push_back(pointer);
                    arrayPointers[symbolAddress] = pointer;

                    if (identifierNode->arrayNumIndex.has_value()) {
                        auto pidentifier2 =
                            identifierNode->arrayNumIndex.value();
                        auto symbol2 = context.symbolTable.getSymbolByName(
                            pidentifier2, currentScope);
                        auto symbolAddress2 = symbol2.address;
                        instructions.emplace_back(SET, symbolAddress);
                        instructions.emplace_back(STORE, pointer, true);
                        instructions.emplace_back(LOAD, symbolAddress2);
                        instructions.emplace_back(ADD, pointer, true);
                        instructions.emplace_back(STORE, pointer, true);
                        lineCounter += 5;
                        addCommand(pidentifier, pointer);
                    }
                    if (identifierNode->arrayPidentifierIndex.has_value()) {
                        auto pidentifier2 =
                            identifierNode->arrayPidentifierIndex.value();
                        auto symbol2 = context.symbolTable.getSymbolByName(
                            pidentifier2, currentScope);
                        auto symbolAddress2 = symbol2.address;
                        instructions.emplace_back(SET, symbolAddress);
                        instructions.emplace_back(STORE, pointer, true);
                        instructions.emplace_back(LOAD, symbolAddress2);
                        instructions.emplace_back(ADD, pointer, true);
                        instructions.emplace_back(STORE, pointer, true);
                        lineCounter += 5;
                        addCommand(pidentifier, pointer);
                    }
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

void CodeGenerator::addHalt() {
    instructions.emplace_back(HALT, 0);
    lineCounter++;
}

void CodeGenerator::addWrite(unsigned long &address) {
    auto it = std::find(heap.begin(), heap.end(), address);
    if (it != heap.end()) {
        instructions.emplace_back(LOADI, address);
        instructions.emplace_back(PUT, 0);
        lineCounter += 2;
        return;
    }
    instructions.emplace_back(PUT, address);
    lineCounter++;
}

void CodeGenerator::addRead(unsigned long &address) {
    auto it = std::find(heap.begin(), heap.end(), address);
    if (it != heap.end()) {
        instructions.emplace_back(GET, 0);
        instructions.emplace_back(STOREI, address);
        lineCounter += 2;
        return;
    }
    instructions.emplace_back(GET, address);
    lineCounter++;
}

void CodeGenerator::addAssign(std::vector<Instruction> &instructions) {
    this->instructions.insert(this->instructions.end(), instructions.begin(),
                              instructions.end());
    lineCounter += instructions.size();
}

void CodeGenerator::addJumpIfAccIsTrue(int &jump) {
    instructions.emplace_back(LOAD, accAddr);  // is this neccessary?
    instructions.emplace_back(JPOS, jump);
    lineCounter += 2;
}

void CodeGenerator::addCommand(std::string &symbolName,
                               unsigned long &address) {
    switch (currentCommand) {
        case READ: {
            addRead(address);
            currentCommand = UNDEFINED;
            break;
        }
        case WRITE: {
            addWrite(address);
            currentCommand = UNDEFINED;
            break;
        }
        case CONDITION: {
            NodeReadyToGenerateCode res = conditionNode.addVariable(address);
            if (res) {
                auto instructions = conditionNode.generateCode();
                addAssign(instructions);
                conditionNode.clear();
                currentCommand = UNDEFINED;
            }
            break;
        }
        case ASSIGN: {
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
            NodeReadyToGenerateCode res = whileNode.addVariable(address);
            if (res) {
                auto instructions = whileNode.generateCode();
                addAssign(instructions);
                whileNode.clear();
                currentCommand = UNDEFINED;
            }
            break;
        }
        case REPEAT: {
            NodeReadyToGenerateCode res = repeatNode.addVariable(address);
            if (res) {
                auto instructions = repeatNode.generateCode();
                addAssign(instructions);
                repeatNode.clear();
                currentCommand = UNDEFINED;
            }
            break;
        }
        case FOR_TO: {
            NodeReadyToGenerateCode res = forNode.addVariable(address);
            if (res) {
                auto instructions = forNode.generateCode();
                addAssign(instructions);
                forNode.clear();
                currentCommand = UNDEFINED;
            }
            break;
        }
        case FOR_DOWNTO: {
            NodeReadyToGenerateCode res = forNode.addVariable(address);
            if (res) {
                auto instructions = forNode.generateCode();
                addAssign(instructions);
                forNode.clear();
                currentCommand = UNDEFINED;
            }
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

void CodeGenerator::setRValues() {
    auto rvalues = context.symbolTable.getRValues();

    for (auto &i : rvalues) {
        instructions.emplace_back(SET, i.name, RVALUE);
        instructions.emplace_back(STORE, i.address);
        lineCounter += 2;
    }
}

long CodeGenerator::getMarkerForName(std::string &name) {
    bool shouldBeIncremented = false;
    bool shouldBeDecremented = false;
    if (name.find(":inc") != std::string::npos) {
        shouldBeIncremented = true;
        name = name.substr(0, name.size() - 4);
    }
    if (name.find(":dec") != std::string::npos) {
        shouldBeDecremented = true;
        name = name.substr(0, name.size() - 4);
    }

    for (auto &i : markers) {
        if (i.name == name) {
            auto jump = i.line;
            if (shouldBeIncremented) jump++;
            if (shouldBeDecremented) jump--;
            return jump;
        }
    }

    throw std::runtime_error("Did not find marker with name: " + name);
}

void CodeGenerator::saveInstructionsToFile() {
    std::ofstream outFile(context.outputFile);
    if (!outFile.is_open()) {
        throw std::runtime_error("Unable to open " + context.outputFile +
                                 " for writing.");
    }

    for (auto &i : instructions) {
        if (i.opcode == HALF || i.opcode == HALT)
            outFile << i.opcode << std::endl;
        else if (i.mode == LABEL) {
            auto jumpOverIf = getMarkerForName(i.label);
            outFile << i.opcode << " " << jumpOverIf << std::endl;
        } else if (i.mode == RVALUE) {
            outFile << i.opcode << " " << i.label << std::endl;
        } else {
            auto it = std::find(heap.begin(), heap.end(), i.value);
            if (it != heap.end() && !i.doNotModify) updateOpcode(i.opcode);
            outFile << i.opcode << " " << i.value << std::endl;
        }
    }
    outFile.close();
}

void CodeGenerator::updateOpcode(Opcode &opcode) {
    switch (opcode) {
        case LOAD: {
            opcode = LOADI;
            break;
        }
        case STORE: {
            opcode = STOREI;
            break;
        }
        case ADD: {
            opcode = ADDI;
            break;
        }
        case SUB: {
            opcode = SUBI;
            break;
        }
        default:
            break;
    }
}

void CodeGenerator::jumpToMain() {
    std::string label = "main";
    auto reduction = lineCounter * -1;
    markers.emplace_back(label, reduction);
    instructions.emplace_back(JUMP, label);
    lineCounter++;
}

bool CodeGenerator::isProcArgument(std::string &argName, int &scope) {
    return context.symbolTable.isProcArgument(argName, currentProcName);
}

}  // namespace codegen
