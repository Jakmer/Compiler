#ifndef CONDITION_NODE_HPP
#define CONDITION_NODE_HPP

#include "ASTNode.hpp"

namespace ast {

using child = ASTNode*;

enum Relation { EQ = 0, NEQ, LT, LE, GT, GE };

class ConditionNode : public ASTNode {
   public:
    child value1;
    child value2;
    Relation relation;

    ConditionNode();

    void print(int indent = 0) const override;
};

}  // namespace ast

#endif  // CONDITION_NODE_HPP
