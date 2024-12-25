#ifndef PROCEDURES_NODE_HPP
#define PROCEDURES_NODE_HPP

#include "ASTNode.hpp"
#include <optional>

namespace ast {

using child = ASTNode*;

class ProceduresNode : public ASTNode {
   public:
    child proc_head;
    std::optional<child> declarations;
    child commands;

    ProceduresNode();

    void print(int indent = 0) const override;
};

class ProcHeadNode : public ASTNode {
   public:
    std::string pidentifier;
    child args_decl;

    ProcHeadNode();

    void print(int indent = 0) const override;
};

class ArgsDeclNode : public ASTNode {
   public:
    std::vector<std::string> pidentifiers;
    std::vector<std::string> Tpidentifiers;

    ArgsDeclNode() = default;

    void print(int indent = 0) const override;
};

}  // namespace ast

#endif  // PROCEDURESNODE_HPP
