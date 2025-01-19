#ifndef VALUE_NODE_HPP
#define VALUE_NODE_HPP

#include <optional>
#include "ASTNode.hpp"

namespace ast {

using child = ASTNode*;

class ValueNode : public ASTNode {
   public:
    std::optional<std::string> num;
    std::optional<child> identifier;

    ValueNode();
    virtual ~ValueNode();

    void print(int indent = 0) const override;
};

}  // namespace ast

#endif  // VALUE_NODE_HPP
