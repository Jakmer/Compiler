#ifndef INSTRUCT_NODES_HPP
#define INSTRUCT_NODES_HPP

#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "Command.hpp"
#include "Instructions.hpp"

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

class AssignNode {
   public:
    AssignNode()
        : steps(0),
          identifier1(0),
          identifier2(0),
          codeGenerated(false),
          waitForThirdArg(false),
          operation(NOT_DEFINED) {}
    ~AssignNode() = default;

    unsigned long identifier1;
    unsigned long identifier2;
    std::optional<unsigned long> identifier3;
    AssignOperation operation;
    bool waitForThirdArg;
    bool codeGenerated;

    std::vector<Instruction> generateCode() {
        if(!waitForThirdArg)
        {
            instructions.emplace_back(LOAD, identifier2);
            instructions.emplace_back(STORE, identifier1);
            codeGenerated = true;
            return instructions;
        }
        
        instructions.emplace_back(LOAD, identifier2);
        if(!identifier3.has_value())
            throw std::runtime_error("Operation needs identifier3!");

        switch (operation) {
            case PLUS:
                instructions.emplace_back(ADD, identifier3.value());
                break;
            case SUBSTRACT:
                instructions.emplace_back(SUB, identifier3.value());
                break;
            case MULTIPLY:
                instructions.emplace_back(ADD, identifier3.value());
                break;
            case DIVIDE:
                break;
            case MOD:
                break;
            case NOT_DEFINED:
                throw std::runtime_error("No operation is set!");
                break;
        }
        instructions.emplace_back(STORE, identifier1);

        codeGenerated = true;
        return instructions;
    }

    NodeReadyToGenerateCode addVariable(unsigned long &var) { 
        if((waitForThirdArg && steps == 3) || (!waitForThirdArg && steps ==2))
            throw std::runtime_error("All variable are set. Code should have been already generated!");

        if(steps == 0)
            identifier1 = var;
        else if(steps==1)
            identifier2 = var;
        else 
            identifier3 = var;

        steps++;

        if((waitForThirdArg && steps == 3) || (!waitForThirdArg && steps ==2))
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
};

}  // namespace codegen

#endif  // !INSTRUCT_NODES_HPP
