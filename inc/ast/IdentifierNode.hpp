#ifndef IDENTIFIER_NODE_HPP
#define IDENTIFIER_NODE_HPP

#include <optional>
#include "ASTNode.hpp"

namespace ast {

class IdentifierNode : public ASTNode {
   public:
    std::optional<std::string> pidentifier;
    std::optional<std::string> Tpidentifier;
    std::optional<int> arrayNumIndex;
    std::optional<std::string> arrayPidentifierIndex;

    IdentifierNode();
    virtual ~IdentifierNode();

    void print(int indent = 0) const override;
};

}  // namespace ast

#endif  // IDENTIFIER_NODE_HPP
