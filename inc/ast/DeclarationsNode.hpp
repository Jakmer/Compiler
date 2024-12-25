#ifndef DECLARATIONS_NODE_HPP
#define DECLARATIONS_NODE_HPP

#include "ASTNode.hpp"

namespace ast {

struct array {
    std::string pidentifier;
    int num1;  // beggining
    int num2;  // end
};

class DeclarationsNode : public ASTNode {
   public:
    std::vector<array> arrays;
    std::vector<std::string> pidentifiers;

    DeclarationsNode();

    void print(int indent = 0) const override;
    void addArray(const std::string& pidentifier, int num1, int num2);
    void addPidentifier(const std::string& pidentifier);
};

}  // namespace ast

#endif  // DECLARATIONS_NODE_HPP
