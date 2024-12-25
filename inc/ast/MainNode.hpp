#ifndef MAIN_NODE_HPP
#define MAIN_NODE_HPP

#include "ASTNode.hpp"

namespace ast {

using child = ASTNode*;

class MainNode : public ASTNode {
   public:
    child declarations;
    child commands;

    MainNode();
    virtual ~MainNode();

    void print(int indent = 0) const override;
};

}  // namespace ast

#endif  // MAIN_NODE_HPP
