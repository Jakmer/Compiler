#ifndef COMMANDS_NODE_HPP
#define COMMANDS_NODE_HPP

#include <memory>
#include <optional>
#include "ASTNode.hpp"

namespace ast {

using child = ASTNode*;

class CommandsNode : public ASTNode {
   public:
    std::vector<child> commands;

    CommandsNode();
    virtual ~CommandsNode();

    void print(int indent = 0) const override;
};

// -- Command --

class AssignmentNode : public ASTNode {
   public:
    child identifier;
    child expression;

    AssignmentNode();
    virtual ~AssignmentNode();

    void print(int indent = 0) const override;
};

class IfStatementNode : public ASTNode {
   public:
    child condition;
    child commands;
    std::optional<child> elseCommands;

    IfStatementNode();
    virtual ~IfStatementNode();

    void print(int indent = 0) const override;
};

class WhileStatementNode : public ASTNode {
   public:
    child condition;
    child commands;

    WhileStatementNode();
    virtual ~WhileStatementNode();

    void print(int indent = 0) const override;
};

class RepeatStatementNode : public ASTNode {
   public:
    child condition;
    child commands;

    RepeatStatementNode();
    virtual ~RepeatStatementNode();

    void print(int indent = 0) const override;
};

class ForToNode : public ASTNode {
   public:
    std::string pidentifier;
    child valueFrom;
    child valueTo;
    child commands;

    ForToNode();
    virtual ~ForToNode();

    void print(int indent = 0) const override;
};

class ForDowntoNode : public ASTNode {
   public:
    std::string pidentifier;
    child valueFrom;
    child valueTo;
    child commands;

    ForDowntoNode();
    virtual ~ForDowntoNode();

    void print(int indent = 0) const override;
};

class ProcCallNode: public ASTNode {
   public:
    std::string pidentifier;
    child args;

    ProcCallNode();
    virtual ~ProcCallNode();

    void print(int indent = 0) const override;
};

class ArgsNode : public ASTNode {
   public:
    std::vector<std::string> pidentifiers;

    ArgsNode();
    virtual ~ArgsNode();

    void print(int indent = 0) const override;
};

class ReadNode : public ASTNode {
   public:
    child identifier;

    ReadNode();
    virtual ~ReadNode();

    void print(int indent = 0) const override;
};

class WriteNode : public ASTNode {
   public:
    child value;

    WriteNode();
    virtual ~WriteNode();

    void print(int indent = 0) const override;
};

}  // namespace ast

#endif  // COMMANDS_NODE_HPP