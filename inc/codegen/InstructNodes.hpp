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
    Node(Memory &memory);
    virtual ~Node() = default;
    virtual std::vector<Instruction> generateCode() = 0;
    virtual NodeReadyToGenerateCode addVariable(unsigned long &var);
    virtual void clear();

    unsigned long identifier1;
    unsigned long identifier2;
    bool codeGenerated;
    std::string name;

   protected:
    int steps;  // counter how many vars we already have - if 2 then generate
    std::vector<Instruction> instructions;
    Memory &memory;
};

class AssignNode : public Node {
   public:
    AssignNode(Memory &memory);
    virtual ~AssignNode() = default;
    std::vector<Instruction> generateCode();
    NodeReadyToGenerateCode addVariable(unsigned long &var);
    void clear();

    std::optional<unsigned long> identifier3;
    AssignOperation operation;
    bool waitForThirdArg;
};

class ConditionNode : public Node {
   public:
    ConditionNode(Memory &memory);
    virtual ~ConditionNode() = default;
    std::vector<Instruction> generateCode();

    ConditionOperation operation;
    bool elseExist;
};

class RepeatNode : public Node {
   public:
    RepeatNode(Memory &memory);
    virtual ~RepeatNode() = default;
    std::vector<Instruction> generateCode();

    ConditionOperation operation;
};

class WhileNode : public Node {
   public:
    WhileNode(Memory &memory);
    virtual ~WhileNode() = default;
    std::vector<Instruction> generateCode();

    ConditionOperation operation;
};

enum ForMode { DOWN_STEP, UP_STEP, NOT_DEF };

class ForNode : public Node {
   public:
    ForNode(Memory &memory);
    virtual ~ForNode() = default;
    std::vector<Instruction> generateCode();
    
    unsigned long iterator;
    ForMode mode;
};

}  // namespace codegen

#endif  // !INSTRUCT_NODES_HPP
