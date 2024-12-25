#ifndef EXPRESSION_NODE_HPP
#define EXPRESSION_NODE_HPP

#include <optional>

#include "ASTNode.hpp"

namespace ast {

using child = ASTNode*;

enum MathOperation { PLUS = 0, SUBSTRACT, MULTIPLY, DIVIDE, MOD };

class ExpressionNode : public ASTNode {
   public:
    child value1;
    std::optional<child> value2;
    std::optional<MathOperation> mathOperation;

    ExpressionNode();
    virtual ~ExpressionNode();

    void print(int indent = 0) const override;
};

}  // namespace ast

#endif  // EXPRESSION_NODE_HPP
