#ifndef INSTRUCT_NODES_HPP
#define INSTRUCT_NODES_HPP

#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "Command.hpp"
#include "Instructions.hpp"
#include "Memory.hpp"

namespace codegen {

using NodeReadyToGenerateCode = bool;

enum AssignOperation {
    PLUS,
    SUBSTRACT,
    MULTIPLY,
    DIVIDE,
    MOD,
    NOT_DEFINED,
};

enum ConditionOperation { EQ, NEQ, GT, LT, GE, LE, UNDEF };

class AssignNode {
   public:
    AssignNode() = default;
    AssignNode(Memory &memory)
        : steps(0),
          identifier1(0),
          identifier2(0),
          codeGenerated(false),
          waitForThirdArg(false),
          memory(memory),
          operation(NOT_DEFINED) {}
    ~AssignNode() = default;

    unsigned long identifier1;
    unsigned long identifier2;
    std::optional<unsigned long> identifier3;
    AssignOperation operation;
    bool waitForThirdArg;
    bool codeGenerated;

    std::vector<Instruction> generateCode() {
        if (!waitForThirdArg) {
            instructions.emplace_back(LOAD, identifier2);
            instructions.emplace_back(STORE, identifier1);
            codeGenerated = true;
            return instructions;
        }

        instructions.emplace_back(LOAD, identifier2);
        if (!identifier3.has_value())
            throw std::runtime_error("Operation needs identifier3!");

        switch (operation) {
            case PLUS: {
                instructions.emplace_back(ADD, identifier3.value());
                break;
            }
            case SUBSTRACT: {
                instructions.emplace_back(SUB, identifier3.value());
                break;
            }
            case MULTIPLY: {
                // Peasant multiplication
                // result will be in acc

                auto freeReg1 = memory.getFreeRegister();
                memory.lockReg(freeReg1);
                instructions.emplace_back(STORE, freeReg1);

                auto freeReg2 = memory.getFreeRegister();
                memory.lockReg(freeReg2);
                instructions.emplace_back(LOAD, identifier3.value());
                instructions.emplace_back(STORE, freeReg2);

                auto freeReg3 = memory.getFreeRegister();
                memory.lockReg(freeReg3);
                // set to 0?

                int jumpToEnd = 17;
                int jumpToEven = 4;
                int jumpToMainLoop = -17;

                // MAIN LOOP
                instructions.emplace_back(LOAD, freeReg1);
                instructions.emplace_back(JZERO, jumpToEnd);  // Jump to end

                // Check if identifier1 is odd
                instructions.emplace_back(LOAD, freeReg1);
                instructions.emplace_back(HALF, freeReg1);
                instructions.emplace_back(LOAD, freeReg1);
                instructions.emplace_back(ADD, freeReg1);
                instructions.emplace_back(SUB, freeReg1);
                instructions.emplace_back(JZERO, jumpToEven);

                // If odd then add identifier2 to result
                instructions.emplace_back(LOAD, freeReg3);
                instructions.emplace_back(ADD, freeReg2);
                instructions.emplace_back(STORE, freeReg3);

                // EVEN
                instructions.emplace_back(LOAD, freeReg1);
                instructions.emplace_back(HALF, freeReg1);
                instructions.emplace_back(STORE, freeReg1);

                // Multiply identifier2 * 2
                instructions.emplace_back(LOAD, freeReg2);
                instructions.emplace_back(ADD, freeReg2);
                instructions.emplace_back(STORE, freeReg2);

                // Jump to main loop
                instructions.emplace_back(JUMP, jumpToMainLoop);

                // END
                instructions.emplace_back(LOAD, freeReg3);

                memory.unlockReg(freeReg1);
                memory.unlockReg(freeReg2);
                memory.unlockReg(freeReg3);

                break;
            }
            case DIVIDE: {
                // Integer division (a / b)
                // Quotient will be in acc, remainder in freeReg3

                auto freeReg1 =
                    memory.getFreeRegister();  // Register for dividend (a)
                memory.lockReg(freeReg1);
                instructions.emplace_back(STORE, freeReg1);

                auto freeReg2 =
                    memory.getFreeRegister();  // Register for divisor (b)
                memory.lockReg(freeReg2);
                instructions.emplace_back(LOAD, identifier3.value());
                instructions.emplace_back(STORE, freeReg2);

                auto freeReg3 =
                    memory.getFreeRegister();  // Register for remainder
                memory.lockReg(freeReg3);
                instructions.emplace_back(SET, 0);
                instructions.emplace_back(STORE, freeReg3);

                auto freeReg4 =
                    memory.getFreeRegister();  // Register for quotient
                memory.lockReg(freeReg4);
                instructions.emplace_back(SET, 0);
                instructions.emplace_back(STORE, freeReg4);

                // Handle division by zero
                instructions.emplace_back(LOAD, freeReg2);
                int jumpToEnd = 21;  // Adjusted jump to the end
                instructions.emplace_back(JZERO, jumpToEnd);

                // MAIN LOOP
                int jumpToSubtract = 6;     // Jump to subtraction
                int jumpToLoopStart = -12;  // Back to loop start

                instructions.emplace_back(LOAD, freeReg1);
                instructions.emplace_back(SUB, freeReg2);  // Check if a >= b
                instructions.emplace_back(JNEG,
                                          jumpToEnd);  // If a < b, exit loop

                // Subtract b from a
                instructions.emplace_back(LOAD, freeReg1);
                instructions.emplace_back(SUB, freeReg2);
                instructions.emplace_back(STORE,
                                          freeReg1);  // Update dividend (a)

                // Increment quotient
                instructions.emplace_back(LOAD, freeReg4);
                instructions.emplace_back(ADD, 1);
                instructions.emplace_back(STORE, freeReg4);

                // Jump back to loop start
                instructions.emplace_back(JUMP, jumpToLoopStart);

                // END
                instructions.emplace_back(LOAD,
                                          freeReg4);  // Load quotient into acc

                // Store remainder
                /*instructions.emplace_back(LOAD, freeReg1);*/
                /*instructions.emplace_back(STORE, freeReg3);*/

                // Unlock registers
                memory.unlockReg(freeReg1);
                memory.unlockReg(freeReg2);
                memory.unlockReg(freeReg3);
                memory.unlockReg(freeReg4);

                break;
            }
            case MOD: {
                // Integer division (a / b)
                // Reminder will be in acc

                auto freeReg1 =
                    memory.getFreeRegister();  // Register for dividend (a)
                memory.lockReg(freeReg1);
                instructions.emplace_back(STORE, freeReg1);

                auto freeReg2 =
                    memory.getFreeRegister();  // Register for divisor (b)
                memory.lockReg(freeReg2);
                instructions.emplace_back(LOAD, identifier3.value());
                instructions.emplace_back(STORE, freeReg2);

                auto freeReg3 =
                    memory.getFreeRegister();  // Register for remainder
                memory.lockReg(freeReg3);
                instructions.emplace_back(SET, 0);
                instructions.emplace_back(STORE, freeReg3);

                auto freeReg4 =
                    memory.getFreeRegister();  // Register for quotient
                memory.lockReg(freeReg4);
                instructions.emplace_back(SET, 0);
                instructions.emplace_back(STORE, freeReg4);

                // Handle division by zero
                instructions.emplace_back(LOAD, freeReg2);
                int jumpToEnd = 21;  // Adjusted jump to the end
                instructions.emplace_back(JZERO, jumpToEnd);

                // MAIN LOOP
                int jumpToSubtract = 6;     // Jump to subtraction
                int jumpToLoopStart = -12;  // Back to loop start

                instructions.emplace_back(LOAD, freeReg1);
                instructions.emplace_back(SUB, freeReg2);  // Check if a >= b
                instructions.emplace_back(JNEG,
                                          jumpToEnd);  // If a < b, exit loop

                // Subtract b from a
                instructions.emplace_back(LOAD, freeReg1);
                instructions.emplace_back(SUB, freeReg2);
                instructions.emplace_back(STORE,
                                          freeReg1);  // Update dividend (a)

                // Increment quotient
                instructions.emplace_back(LOAD, freeReg4);
                instructions.emplace_back(ADD, 1);
                instructions.emplace_back(STORE, freeReg4);

                // Jump back to loop start
                instructions.emplace_back(JUMP, jumpToLoopStart);

                // END
                instructions.emplace_back(LOAD,
                                          freeReg4);  // Load remainder into acc

                // Store remainder
                /*instructions.emplace_back(LOAD, freeReg1);*/
                /*instructions.emplace_back(STORE, freeReg3);*/

                // Unlock registers
                memory.unlockReg(freeReg1);
                memory.unlockReg(freeReg2);
                memory.unlockReg(freeReg3);
                memory.unlockReg(freeReg4);
                break;
            }
            case NOT_DEFINED:
                throw std::runtime_error("No operation is set!");
                break;
        }
        instructions.emplace_back(STORE, identifier1);

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

   private:
    int steps;  // counter how many vars we already have - if 3 then generate
    std::vector<Instruction> instructions;
    Memory memory;
};

class ConditionNode {
   public:
    unsigned long identifier1;
    unsigned long identifier2;
    ConditionOperation operation;
    bool elseExist;
    std::string name;

    ConditionNode() = default;

    ConditionNode(Memory &memory)
        : steps(0),
          identifier1(0),
          identifier2(0),
          codeGenerated(false),
          memory(memory),
          elseExist(false),
          operation(UNDEF) {}

    ~ConditionNode() = default;

    std::vector<Instruction> generateCode() {  // store result in acc
        auto freeReg2 = memory.getFreeRegister();
        memory.lockReg(freeReg2);
        instructions.emplace_back(LOAD, identifier2);
        instructions.emplace_back(STORE, freeReg2);

        auto freeReg1 = memory.getFreeRegister();
        memory.lockReg(freeReg1);
        instructions.emplace_back(LOAD, identifier1);
        instructions.emplace_back(STORE, freeReg1);

        std::string jumpOverIf = name;  // during generation replace jumpOverIf labe with marker standing by particular if label
        std::string jumpOverIfIncremented = name+":inc";    // bc we have jump over "jumpOverIf" also

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
    NodeReadyToGenerateCode addVariable(unsigned long &var) {
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

    void clear() {
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

   private:
    int steps;
    bool codeGenerated;
    Memory memory;
    std::vector<Instruction> instructions;
};

}  // namespace codegen

#endif  // !INSTRUCT_NODES_HPP
