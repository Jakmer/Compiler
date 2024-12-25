#ifndef PROGRAM_ALL_NODE_HPP
#define PROGRAM_ALL_NODE_HPP

#include "ASTNode.hpp"

namespace ast {

using child = ASTNode*;

class ProgramAllNode : public ASTNode {
   public:
    std::vector<child> procedures;
    child main;

    ProgramAllNode();

    void print(int indent = 0) const override;
};

}  // namespace ast

#endif  // PROGRAM_ALL_NODE_HPP
