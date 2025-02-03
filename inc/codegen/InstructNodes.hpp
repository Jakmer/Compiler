#ifndef INSTRUCT_NODES_HPP
#define INSTRUCT_NODES_HPP

#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "Command.hpp"
#include "IdentifierNode.hpp"
#include "Instructions.hpp"
#include "Memory.hpp"

namespace codegen {

// TODO: Nodes below are very redundant - make it more reusable

using NodeReadyToGenerateCode = bool;

enum AssignOperation {
    PLUS,
    SUBSTRACT,
    MULTIPLY,
    DIVIDE,
    MOD,
    NOT_DEFINED,
};

enum ConditionOperation { EQ = 0, NEQ, LT, LE, GT, GE, UNDEF };

class Node {
   public:
    Node() = default;
    Node(Memory &memory)
        : steps(0),
          identifier1(0),
          identifier2(0),
          codeGenerated(false),
          memory(memory) {}
    virtual ~Node() = default;

    unsigned long identifier1;
    unsigned long identifier2;
    bool codeGenerated;
    std::string name;

    virtual std::vector<Instruction> generateCode() = 0;

    virtual NodeReadyToGenerateCode addVariable(unsigned long &var) {
        if (steps == 2)
            throw std::runtime_error(
                "All variable are set. Code should have been already "
                "generated!");

        if (steps == 0)
            identifier1 = var;
        else
            identifier2 = var;

        steps++;

        return steps == 2 ? true : false;
    }

    virtual void clear() {
        if (!codeGenerated)
            throw std::runtime_error(
                "Cannot clear until code from node will be generated! Finish "
                "assignment first!");
        identifier1 = 0;
        identifier2 = 0;
        codeGenerated = false;
        steps = 0;
        instructions.clear();
    }

   protected:
    int steps;  // counter how many vars we already have - if 2 then generate
    std::vector<Instruction> instructions;
    Memory &memory;
};

class AssignNode : public Node {
   public:
    AssignNode(Memory &memory)
        : Node(memory), waitForThirdArg(false), operation(NOT_DEFINED) {}
    virtual ~AssignNode() = default;

    std::optional<unsigned long> identifier3;
    AssignOperation operation;
    bool waitForThirdArg;

    std::vector<Instruction> generateCode() {
        if (!waitForThirdArg) {
            instructions.emplace_back(LOAD, identifier2);
            instructions.emplace_back(STORE, identifier1);
            codeGenerated = true;
            return instructions;
        }

        if (!identifier3.has_value())
            throw std::runtime_error("Operation needs identifier3!");

        switch (operation) {
            case PLUS: {
                instructions.emplace_back(LOAD, identifier2);
                instructions.emplace_back(ADD, identifier3.value());
                instructions.emplace_back(STORE, identifier1);
                break;
            }
            case SUBSTRACT: {
                instructions.emplace_back(LOAD, identifier2);
                instructions.emplace_back(SUB, identifier3.value());
                instructions.emplace_back(STORE, identifier1);
                break;
            }
            case MULTIPLY: {
                auto freeReg1 = memory.getFreeRegister();
                memory.lockReg(freeReg1);
                instructions.emplace_back(LOAD, identifier2);
                instructions.emplace_back(STORE, freeReg1, true);

                auto freeReg2 = memory.getFreeRegister();
                memory.lockReg(freeReg2);
                instructions.emplace_back(LOAD, identifier3.value());
                instructions.emplace_back(STORE, freeReg2, true);

                auto freeReg3 = memory.getFreeRegister();
                memory.lockReg(freeReg3);
                auto freeReg4 = memory.getFreeRegister();
                memory.lockReg(freeReg4);
                auto freeReg5 = memory.getFreeRegister();
                memory.lockReg(freeReg5);
                auto freeReg6 = memory.getFreeRegister();
                memory.lockReg(freeReg6);

                int jump1 = 46;
                int jump2 = 40;
                int jump3 = 19;
                int jump4 = -20;

                std::cout<<"p1: "<<identifier1<<"\n";
                instructions.emplace_back(LOAD, freeReg1, true);
                instructions.emplace_back(JZERO, jump1, true);
                instructions.emplace_back(JPOS, 3, true);
                instructions.emplace_back(SUB, freeReg1, true);
                instructions.emplace_back(SUB, freeReg1, true);
                instructions.emplace_back(STORE, freeReg3, true);
                instructions.emplace_back(LOAD, freeReg2, true);
                instructions.emplace_back(JZERO, jump2, true);
                instructions.emplace_back(JPOS, 3, true);
                instructions.emplace_back(SUB, freeReg2, true);
                instructions.emplace_back(SUB, freeReg2, true);
                instructions.emplace_back(STORE, freeReg4, true);
                instructions.emplace_back(SET, 0, true);
                instructions.emplace_back(STORE, freeReg5, true);
                instructions.emplace_back(LOAD, freeReg4, true);
                instructions.emplace_back(JPOS, 2, true);
                instructions.emplace_back(JUMP, jump3, true);
                instructions.emplace_back(HALF, 0, true);
                instructions.emplace_back(ADD, 0, true);
                instructions.emplace_back(SUB, freeReg4, true);
                instructions.emplace_back(STORE, freeReg6, true);
                instructions.emplace_back(SET, 1, true);
                instructions.emplace_back(ADD, freeReg6, true);
                instructions.emplace_back(JZERO, 2, true);
                instructions.emplace_back(JUMP, 4, true);
                instructions.emplace_back(LOAD, freeReg5, true);
                instructions.emplace_back(ADD, freeReg3, true);
                instructions.emplace_back(STORE, freeReg5, true);
                instructions.emplace_back(LOAD, freeReg3, true);
                instructions.emplace_back(ADD, freeReg3, true);
                instructions.emplace_back(STORE, freeReg3, true);
                instructions.emplace_back(LOAD, freeReg4, true);
                instructions.emplace_back(HALF, 0, true);
                instructions.emplace_back(STORE, freeReg4, true);
                instructions.emplace_back(JUMP, jump4, true);
                instructions.emplace_back(LOAD, freeReg1, true);
                instructions.emplace_back(JPOS, 4, true);
                instructions.emplace_back(LOAD, freeReg2, true);
                instructions.emplace_back(JNEG, 8, true);
                instructions.emplace_back(JUMP, 3, true);
                instructions.emplace_back(LOAD, freeReg2, true);
                instructions.emplace_back(JPOS, 5, true);
                instructions.emplace_back(LOAD, freeReg5, true);
                instructions.emplace_back(SUB, freeReg5, true);
                instructions.emplace_back(SUB, freeReg5, true);
                instructions.emplace_back(JUMP, 2, true);
                instructions.emplace_back(LOAD, freeReg5, true);
                instructions.emplace_back(STORE, identifier1);

                memory.unlockReg(freeReg1);
                memory.unlockReg(freeReg2);
                memory.unlockReg(freeReg3);
                memory.unlockReg(freeReg4);
                memory.unlockReg(freeReg5);
                memory.unlockReg(freeReg6);

                break;
            }
            case DIVIDE: {
                auto dividend = memory.getFreeRegister();
                memory.lockReg(dividend);
                instructions.emplace_back(LOAD, identifier2);
                instructions.emplace_back(STORE, dividend, true);

                auto divisor = memory.getFreeRegister();
                memory.lockReg(divisor);
                instructions.emplace_back(LOAD, identifier3.value());
                instructions.emplace_back(STORE, divisor);

                auto freeReg3 = memory.getFreeRegister();
                memory.lockReg(freeReg3);
                auto freeReg4 = memory.getFreeRegister();
                memory.lockReg(freeReg4);
                auto freeReg5 = memory.getFreeRegister();
                memory.lockReg(freeReg5);
                auto freeReg6 = memory.getFreeRegister();
                memory.lockReg(freeReg6);
                auto freeReg7 = memory.getFreeRegister();
                memory.lockReg(freeReg7);
                auto freeReg8 = memory.getFreeRegister();
                memory.lockReg(freeReg8);
                auto freeReg9 = memory.getFreeRegister();
                memory.lockReg(freeReg9);
                auto freeReg10 = memory.getFreeRegister();
                memory.lockReg(freeReg10);

                instructions.emplace_back(SET, 1, true);
                instructions.emplace_back(STORE, freeReg3, true);
                instructions.emplace_back(SET, 0, true);
                instructions.emplace_back(STORE, freeReg4, true);
                /*instructions.emplace_back(GET, dividend, true);*/
                /*instructions.emplace_back(GET, divisor, true);*/
                instructions.emplace_back(LOAD, freeReg4, true);
                instructions.emplace_back(STORE, freeReg5, true);
                instructions.emplace_back(LOAD, divisor, true);
                instructions.emplace_back(STORE, freeReg6, true);
                instructions.emplace_back(SUB, freeReg5, true);
                instructions.emplace_back(JPOS, 5, true);
                instructions.emplace_back(JNEG, 4, true);
                instructions.emplace_back(LOAD, freeReg4, true);
                instructions.emplace_back(STORE, freeReg7, true);
                instructions.emplace_back(JUMP, 112, true);
                instructions.emplace_back(LOAD, freeReg4, true);
                instructions.emplace_back(STORE, freeReg8, true);
                instructions.emplace_back(LOAD, freeReg4, true);
                instructions.emplace_back(STORE, freeReg6, true);
                instructions.emplace_back(LOAD, dividend, true);
                instructions.emplace_back(STORE, freeReg5, true);
                instructions.emplace_back(SUB, freeReg6, true);
                instructions.emplace_back(JZERO, 4, true);
                instructions.emplace_back(JPOS, 3, true);
                instructions.emplace_back(LOAD, freeReg3, true);
                instructions.emplace_back(STORE, freeReg8, true);
                instructions.emplace_back(LOAD, freeReg4, true);
                instructions.emplace_back(STORE, freeReg6, true);
                instructions.emplace_back(LOAD, divisor, true);
                instructions.emplace_back(STORE, freeReg5, true);
                instructions.emplace_back(SUB, freeReg6, true);
                instructions.emplace_back(JZERO, 5, true);
                instructions.emplace_back(JPOS, 4, true);
                instructions.emplace_back(LOAD, freeReg8, true);
                instructions.emplace_back(ADD, freeReg3, true);
                instructions.emplace_back(STORE, freeReg8, true);
                instructions.emplace_back(LOAD, freeReg4, true);
                instructions.emplace_back(STORE, freeReg6, true);
                instructions.emplace_back(LOAD, dividend, true);
                instructions.emplace_back(STORE, freeReg5, true);
                instructions.emplace_back(SUB, freeReg6, true);
                instructions.emplace_back(JZERO, 10, true);
                instructions.emplace_back(JPOS, 9, true);
                instructions.emplace_back(LOAD, dividend, true);
                instructions.emplace_back(STORE, freeReg9, true);
                instructions.emplace_back(LOAD, freeReg9, true);
                instructions.emplace_back(SUB, freeReg9, true);
                instructions.emplace_back(STORE, dividend, true);
                instructions.emplace_back(LOAD, dividend, true);
                instructions.emplace_back(SUB, freeReg9, true);
                instructions.emplace_back(STORE, dividend, true);
                instructions.emplace_back(LOAD, freeReg4, true);
                instructions.emplace_back(STORE, freeReg6, true);
                instructions.emplace_back(LOAD, divisor, true);
                instructions.emplace_back(STORE, freeReg5, true);
                instructions.emplace_back(SUB, freeReg6, true);
                instructions.emplace_back(JZERO, 10, true);
                instructions.emplace_back(JPOS, 9, true);
                instructions.emplace_back(LOAD, divisor, true);
                instructions.emplace_back(STORE, freeReg9, true);
                instructions.emplace_back(LOAD, freeReg9, true);
                instructions.emplace_back(SUB, freeReg9, true);
                instructions.emplace_back(STORE, divisor, true);
                instructions.emplace_back(LOAD, divisor, true);
                instructions.emplace_back(SUB, freeReg9, true);
                instructions.emplace_back(STORE, divisor, true);
                instructions.emplace_back(LOAD, freeReg4, true);
                instructions.emplace_back(STORE, freeReg7, true);
                instructions.emplace_back(LOAD, freeReg3, true);
                instructions.emplace_back(STORE, freeReg10, true);
                instructions.emplace_back(LOAD, divisor, true);
                instructions.emplace_back(STORE, freeReg9, true);
                instructions.emplace_back(LOAD, dividend, true);
                instructions.emplace_back(STORE, freeReg5, true);
                instructions.emplace_back(LOAD, freeReg9, true);
                instructions.emplace_back(STORE, freeReg6, true);
                instructions.emplace_back(SUB, freeReg5, true);
                instructions.emplace_back(JPOS, 8, true);
                instructions.emplace_back(LOAD, freeReg9, true);
                instructions.emplace_back(ADD, freeReg9, true);
                instructions.emplace_back(STORE, freeReg9, true);
                instructions.emplace_back(LOAD, freeReg10, true);
                instructions.emplace_back(ADD, freeReg10, true);
                instructions.emplace_back(STORE, freeReg10, true);
                instructions.emplace_back(JUMP, -12, true);
                instructions.emplace_back(LOAD, freeReg3, true);
                instructions.emplace_back(STORE, freeReg6, true);
                instructions.emplace_back(LOAD, freeReg10, true);
                instructions.emplace_back(STORE, freeReg5, true);
                instructions.emplace_back(SUB, freeReg6, true);
                instructions.emplace_back(JNEG, 21, true);
                instructions.emplace_back(JZERO, 20, true);
                instructions.emplace_back(LOAD, freeReg9, true);
                instructions.emplace_back(HALF, true);
                instructions.emplace_back(STORE, freeReg9, true);
                instructions.emplace_back(LOAD, freeReg10, true);
                instructions.emplace_back(HALF, true);
                instructions.emplace_back(STORE, freeReg10, true);
                instructions.emplace_back(LOAD, freeReg9, true);
                instructions.emplace_back(STORE, freeReg6, true);
                instructions.emplace_back(LOAD, dividend, true);
                instructions.emplace_back(STORE, freeReg5, true);
                instructions.emplace_back(SUB, freeReg6, true);
                instructions.emplace_back(JNEG, 7, true);
                instructions.emplace_back(LOAD, dividend, true);
                instructions.emplace_back(SUB, freeReg9, true);
                instructions.emplace_back(STORE, dividend, true);
                instructions.emplace_back(LOAD, freeReg7, true);
                instructions.emplace_back(ADD, freeReg10, true);
                instructions.emplace_back(STORE, freeReg7, true);
                instructions.emplace_back(JUMP, -25, true);
                instructions.emplace_back(LOAD, freeReg3, true);
                instructions.emplace_back(STORE, freeReg6, true);
                instructions.emplace_back(LOAD, freeReg8, true);
                instructions.emplace_back(STORE, freeReg5, true);
                instructions.emplace_back(SUB, freeReg6, true);
                instructions.emplace_back(JPOS, 10, true);
                instructions.emplace_back(JNEG, 9, true);
                instructions.emplace_back(LOAD, freeReg7, true);
                instructions.emplace_back(STORE, freeReg9, true);
                instructions.emplace_back(LOAD, freeReg7, true);
                instructions.emplace_back(SUB, freeReg9, true);
                instructions.emplace_back(STORE, freeReg7, true);
                instructions.emplace_back(LOAD, freeReg7, true);
                instructions.emplace_back(SUB, freeReg9, true);
                instructions.emplace_back(STORE, freeReg7, true);
                instructions.emplace_back(LOAD, freeReg7, true);
                instructions.emplace_back(STORE, identifier1);

                // Unlock registers
                memory.unlockReg(freeReg3);
                memory.unlockReg(freeReg4);
                memory.unlockReg(freeReg5);
                memory.unlockReg(freeReg6);
                memory.unlockReg(freeReg7);
                memory.unlockReg(freeReg8);
                memory.unlockReg(freeReg9);
                memory.unlockReg(freeReg10);

                break;
            }
            case MOD: {
                auto dividend = memory.getFreeRegister();
                memory.lockReg(dividend);
                instructions.emplace_back(LOAD, identifier2);
                instructions.emplace_back(STORE, dividend, true);

                auto divisor = memory.getFreeRegister();
                memory.lockReg(divisor);
                instructions.emplace_back(LOAD, identifier3.value());
                instructions.emplace_back(STORE, divisor, true);

                auto reg1 = memory.getFreeRegister();
                memory.lockReg(reg1);
                auto reg2 = memory.getFreeRegister();
                memory.lockReg(reg2);
                auto reg3 = memory.getFreeRegister();
                memory.lockReg(reg3);
                auto reg4 = memory.getFreeRegister();
                memory.lockReg(reg4);
                auto reg5 = memory.getFreeRegister();
                memory.lockReg(reg5);
                auto reg6 = memory.getFreeRegister();
                memory.lockReg(reg6);
                auto reg7 = memory.getFreeRegister();
                memory.lockReg(reg7);
                auto reg8 = memory.getFreeRegister();
                memory.lockReg(reg8);
                auto reg9 = memory.getFreeRegister();
                memory.lockReg(reg9);
                auto reg10 = memory.getFreeRegister();
                memory.lockReg(reg10);

                instructions.emplace_back(SET, 1, true);
                instructions.emplace_back(STORE, reg1, true);
                instructions.emplace_back(SET, 0, true);
                instructions.emplace_back(STORE, reg2, true);
                /*instructions.emplace_back(GET, dividend, true);*/
                /*instructions.emplace_back(GET, divisor, true);*/
                instructions.emplace_back(LOAD, dividend, true);
                instructions.emplace_back(STORE, reg3, true);
                instructions.emplace_back(LOAD, divisor, true);
                instructions.emplace_back(STORE, reg4, true);
                instructions.emplace_back(LOAD, reg2, true);
                instructions.emplace_back(STORE, reg5, true);
                instructions.emplace_back(LOAD, divisor, true);
                instructions.emplace_back(STORE, reg6, true);
                instructions.emplace_back(SUB, reg5, true);
                instructions.emplace_back(JPOS, 5, true);
                instructions.emplace_back(JNEG, 4, true);
                instructions.emplace_back(LOAD, reg2, true);
                instructions.emplace_back(STORE, reg7, true);
                instructions.emplace_back(JUMP, 94, true);
                instructions.emplace_back(LOAD, reg2, true);
                instructions.emplace_back(STORE, reg6, true);
                instructions.emplace_back(LOAD, dividend, true);
                instructions.emplace_back(STORE, reg5, true);
                instructions.emplace_back(SUB, reg6, true);
                instructions.emplace_back(JZERO, 5, true);
                instructions.emplace_back(JPOS, 4, true);
                instructions.emplace_back(LOAD, reg2, true);
                instructions.emplace_back(SUB, dividend, true);
                instructions.emplace_back(STORE, dividend, true);
                instructions.emplace_back(LOAD, reg2, true);
                instructions.emplace_back(STORE, reg6, true);
                instructions.emplace_back(LOAD, divisor, true);
                instructions.emplace_back(STORE, reg5, true);
                instructions.emplace_back(SUB, reg6, true);
                instructions.emplace_back(JZERO, 5, true);
                instructions.emplace_back(JPOS, 4, true);
                instructions.emplace_back(LOAD, reg2, true);
                instructions.emplace_back(SUB, divisor, true);
                instructions.emplace_back(STORE, divisor, true);
                instructions.emplace_back(LOAD, reg2, true);
                instructions.emplace_back(STORE, reg10, true);
                instructions.emplace_back(LOAD, reg1, true);
                instructions.emplace_back(STORE, reg9, true);
                instructions.emplace_back(LOAD, divisor, true);
                instructions.emplace_back(STORE, reg8, true);
                instructions.emplace_back(LOAD, dividend, true);
                instructions.emplace_back(STORE, reg5, true);
                instructions.emplace_back(LOAD, reg8, true);
                instructions.emplace_back(STORE, reg6, true);
                instructions.emplace_back(SUB, reg5, true);
                instructions.emplace_back(JPOS, 8, true);
                instructions.emplace_back(LOAD, reg8, true);
                instructions.emplace_back(ADD, reg8, true);
                instructions.emplace_back(STORE, reg8, true);
                instructions.emplace_back(LOAD, reg9, true);
                instructions.emplace_back(ADD, reg9, true);
                instructions.emplace_back(STORE, reg9, true);
                instructions.emplace_back(JUMP, -12, true);
                instructions.emplace_back(LOAD, reg1, true);
                instructions.emplace_back(STORE, reg6, true);
                instructions.emplace_back(LOAD, reg9, true);
                instructions.emplace_back(STORE, reg5, true);
                instructions.emplace_back(SUB, reg6, true);
                instructions.emplace_back(JNEG, 21, true);
                instructions.emplace_back(JZERO, 20, true);
                instructions.emplace_back(LOAD, reg8, true);
                instructions.emplace_back(HALF, true);
                instructions.emplace_back(STORE, reg8, true);
                instructions.emplace_back(LOAD, reg9, true);
                instructions.emplace_back(HALF, true);
                instructions.emplace_back(STORE, reg9, true);
                instructions.emplace_back(LOAD, reg8, true);
                instructions.emplace_back(STORE, reg6, true);
                instructions.emplace_back(LOAD, dividend, true);
                instructions.emplace_back(STORE, reg5, true);
                instructions.emplace_back(SUB, reg6, true);
                instructions.emplace_back(JNEG, 7, true);
                instructions.emplace_back(LOAD, dividend, true);
                instructions.emplace_back(SUB, reg8, true);
                instructions.emplace_back(STORE, dividend, true);
                instructions.emplace_back(LOAD, reg10, true);
                instructions.emplace_back(ADD, reg9, true);
                instructions.emplace_back(STORE, reg10, true);
                instructions.emplace_back(JUMP, -25, true);
                instructions.emplace_back(LOAD, dividend, true);
                instructions.emplace_back(STORE, reg8, true);
                instructions.emplace_back(LOAD, reg2, true);
                instructions.emplace_back(STORE, reg6, true);
                instructions.emplace_back(LOAD, reg8, true);
                instructions.emplace_back(STORE, reg5, true);
                instructions.emplace_back(SUB, reg6, true);
                instructions.emplace_back(JZERO, 21, true);
                instructions.emplace_back(LOAD, reg2, true);
                instructions.emplace_back(STORE, reg6, true);
                instructions.emplace_back(LOAD, reg3, true);
                instructions.emplace_back(STORE, reg5, true);
                instructions.emplace_back(SUB, reg6, true);
                instructions.emplace_back(JZERO, 5, true);
                instructions.emplace_back(JPOS, 4, true);
                instructions.emplace_back(LOAD, divisor, true);
                instructions.emplace_back(SUB, reg8, true);
                instructions.emplace_back(STORE, reg8, true);
                instructions.emplace_back(LOAD, reg2, true);
                instructions.emplace_back(STORE, reg6, true);
                instructions.emplace_back(LOAD, reg4, true);
                instructions.emplace_back(STORE, reg5, true);
                instructions.emplace_back(SUB, reg6, true);
                instructions.emplace_back(JZERO, 5, true);
                instructions.emplace_back(JPOS, 4, true);
                instructions.emplace_back(LOAD, reg8, true);
                instructions.emplace_back(SUB, divisor, true);
                instructions.emplace_back(STORE, reg8, true);
                instructions.emplace_back(LOAD, reg8, true);
                instructions.emplace_back(STORE, identifier1);

                // Unlock registers
                memory.unlockReg(reg1);
                memory.unlockReg(reg2);
                memory.unlockReg(reg3);
                memory.unlockReg(reg4);
                memory.unlockReg(reg5);
                memory.unlockReg(reg6);
                memory.unlockReg(reg7);
                memory.unlockReg(reg8);
                memory.unlockReg(reg9);
                memory.unlockReg(reg10);

                break;
            }
            case NOT_DEFINED:
                throw std::runtime_error("No operation is set!");
                break;
        }

        codeGenerated = true;
        return instructions;
    }

    NodeReadyToGenerateCode addVariable(unsigned long &var) {
        if ((waitForThirdArg && steps == 3) || (!waitForThirdArg && steps == 2))
            throw std::runtime_error(
                "All variable are set. Code should have been already "
                "generated!");

        if (steps == 0)
            identifier1 = var;
        else if (steps == 1)
            identifier2 = var;
        else
            identifier3 = var;

        steps++;

        if ((waitForThirdArg && steps == 3) || (!waitForThirdArg && steps == 2))
            return true;
        return false;
    }

    void clear() {
        if (!codeGenerated)
            throw std::runtime_error(
                "Cannot clear until code from node will be generated! Finish "
                "assignment first!");
        identifier1 = 0;
        identifier2 = 0;
        identifier3.reset();
        operation = NOT_DEFINED;
        waitForThirdArg = false;
        codeGenerated = false;
        steps = 0;
        instructions.clear();
    }
};

class ConditionNode : public Node {
   public:
    ConditionOperation operation;
    bool elseExist;

    ConditionNode(Memory &memory)
        : Node(memory), elseExist(false), operation(UNDEF) {}
    virtual ~ConditionNode() = default;

    std::vector<Instruction> generateCode() {  // store result in acc
        auto freeReg2 = memory.getFreeRegister();
        memory.lockReg(freeReg2);
        instructions.emplace_back(LOAD, identifier2);
        instructions.emplace_back(STORE, freeReg2);

        auto freeReg1 = memory.getFreeRegister();
        memory.lockReg(freeReg1);
        instructions.emplace_back(LOAD, identifier1);
        instructions.emplace_back(STORE, freeReg1);

        std::string jumpOverIf =
            name;  // during generation replace jumpOverIf labe with marker
                   // standing by particular if label
        std::string jumpOverIfIncremented =
            name + ":inc";  // bc we have jump over "jumpOverIf" also

        switch (operation) {
            case EQ: {
                instructions.emplace_back(SUB, freeReg2);
                // JPOS jump if !=0
                // JNEG jump if !=0
                instructions.emplace_back(JPOS, jumpOverIfIncremented);
                instructions.emplace_back(JNEG, jumpOverIf);
                break;
            }
            case NEQ: {
                instructions.emplace_back(SUB, freeReg2);
                // JZERO    jump only if acc == 0
                instructions.emplace_back(JZERO, jumpOverIf);
                break;
            }
            case GT: {
                instructions.emplace_back(SUB, freeReg2);
                // JZERO    jump only if acc >= 0
                // JNEG
                instructions.emplace_back(JZERO, jumpOverIfIncremented);
                instructions.emplace_back(JNEG, jumpOverIf);
                break;
            }
            case LT: {
                instructions.emplace_back(SUB, freeReg2);
                // JZERO    jump only if acc <= 0
                // JPOS
                instructions.emplace_back(JZERO, jumpOverIfIncremented);
                instructions.emplace_back(JPOS, jumpOverIf);
                break;
            }
            case GE: {
                instructions.emplace_back(SUB, freeReg2);
                // JNEG     jump only if acc < 0
                instructions.emplace_back(JNEG, jumpOverIf);
                break;
            }
            case LE: {
                instructions.emplace_back(SUB, freeReg2);
                // JPOS    jump only if acc > 0
                instructions.emplace_back(JPOS, jumpOverIf);
                break;
            }
            case UNDEF: {
                throw std::runtime_error("No operation is set!");
                break;
            }
        }

        memory.unlockReg(freeReg1);
        memory.unlockReg(freeReg2);

        codeGenerated = true;

        return instructions;
    }
};

class RepeatNode : public Node {
   public:
    ConditionOperation operation;

    RepeatNode(Memory &memory) : Node(memory), operation(UNDEF) {}
    virtual ~RepeatNode() = default;

    std::vector<Instruction> generateCode() {  // store result in acc
        auto freeReg2 = memory.getFreeRegister();
        memory.lockReg(freeReg2);
        instructions.emplace_back(LOAD, identifier2);
        instructions.emplace_back(STORE, freeReg2);

        auto freeReg1 = memory.getFreeRegister();
        memory.lockReg(freeReg1);
        instructions.emplace_back(LOAD, identifier1);
        instructions.emplace_back(STORE, freeReg1);

        std::string jumpToBegining = name;
        std::string jumpToBeginingInc = name + ":inc";

        switch (operation) {
            case EQ: {
                instructions.emplace_back(SUB, freeReg2);
                instructions.emplace_back(JPOS, jumpToBeginingInc);
                instructions.emplace_back(JNEG, jumpToBegining);
                break;
            }
            case NEQ: {
                instructions.emplace_back(SUB, freeReg2);
                instructions.emplace_back(JZERO, jumpToBegining);
                break;
            }
            case GT: {
                instructions.emplace_back(SUB, freeReg2);
                instructions.emplace_back(JNEG, jumpToBeginingInc);
                instructions.emplace_back(JPOS, jumpToBegining);
                break;
            }
            case LT: {
                instructions.emplace_back(SUB, freeReg2);
                instructions.emplace_back(JZERO, jumpToBeginingInc);
                instructions.emplace_back(JPOS, jumpToBegining);
                break;
            }
            case GE: {
                instructions.emplace_back(SUB, freeReg2);
                instructions.emplace_back(JNEG, jumpToBeginingInc);
                break;
            }
            case LE: {
                instructions.emplace_back(SUB, freeReg2);
                instructions.emplace_back(JPOS, jumpToBegining);
                break;
            }
            case UNDEF: {
                throw std::runtime_error("No operation is set!");
                break;
            }
        }

        memory.unlockReg(freeReg1);
        memory.unlockReg(freeReg2);

        codeGenerated = true;

        return instructions;
    }
};

class WhileNode : public Node {
   public:
    ConditionOperation operation;

    WhileNode(Memory &memory) : Node(memory), operation(UNDEF) {}
    virtual ~WhileNode() = default;

    std::vector<Instruction> generateCode() {  // store result in acc
        auto freeReg2 = memory.getFreeRegister();
        memory.lockReg(freeReg2);
        instructions.emplace_back(LOAD, identifier2);
        instructions.emplace_back(STORE, freeReg2);

        auto freeReg1 = memory.getFreeRegister();
        memory.lockReg(freeReg1);
        instructions.emplace_back(LOAD, identifier1);
        instructions.emplace_back(STORE, freeReg1);

        std::string jumpOverCommands = name;
        std::string jumpOverCommandsInc = name + ":inc";

        switch (operation) {
            case EQ: {
                instructions.emplace_back(SUB, freeReg2);
                instructions.emplace_back(JPOS, jumpOverCommandsInc);
                instructions.emplace_back(JNEG, jumpOverCommands);
                break;
            }
            case NEQ: {
                instructions.emplace_back(SUB, freeReg2);
                instructions.emplace_back(JZERO, jumpOverCommands);
                break;
            }
            case GT: {
                instructions.emplace_back(SUB, freeReg2);
                instructions.emplace_back(JNEG, jumpOverCommandsInc);
                instructions.emplace_back(JZERO, jumpOverCommands);
                break;
            }
            case LT: {
                instructions.emplace_back(SUB, freeReg2);
                instructions.emplace_back(JPOS, jumpOverCommandsInc);
                instructions.emplace_back(JZERO, jumpOverCommands);
                break;
            }
            case GE: {
                instructions.emplace_back(SUB, freeReg2);
                instructions.emplace_back(JNEG, jumpOverCommands);
                break;
            }
            case LE: {
                instructions.emplace_back(SUB, freeReg2);
                instructions.emplace_back(JPOS, jumpOverCommands);
                break;
            }
            case UNDEF: {
                throw std::runtime_error("No operation is set!");
                break;
            }
        }

        memory.unlockReg(freeReg1);
        memory.unlockReg(freeReg2);

        codeGenerated = true;

        return instructions;
    }
};

enum ForMode { DOWN_STEP, UP_STEP, NOT_DEF };

class ForNode : public Node {
   public:
    unsigned long iterator;
    ForMode mode;

    ForNode(Memory &memory) : Node(memory), iterator(0), mode(NOT_DEF) {}
    virtual ~ForNode() = default;

    std::vector<Instruction> generateCode() {  // store result in acc

        // assign iterator to from_val
        instructions.emplace_back(LOAD, identifier1);
        instructions.emplace_back(STORE, iterator);

        std::string jumpOverCommands = name;
        std::string jumpOverCommandsInc = name + ":inc";

        // check if iterator reached to_val
        instructions.emplace_back(LOAD, iterator);
        instructions.emplace_back(SUB, identifier2);
        if (mode == UP_STEP)
            instructions.emplace_back(JPOS, jumpOverCommands);
        else
            instructions.emplace_back(JNEG, jumpOverCommands);

        codeGenerated = true;

        return instructions;
    }
};

}  // namespace codegen

#endif  // !INSTRUCT_NODES_HPP
