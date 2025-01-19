#ifndef DECLARATIONS_NODE_HPP
#define DECLARATIONS_NODE_HPP

#include "ASTNode.hpp"

namespace ast {

struct array {
    std::string pidentifier;
    std::string num1;  // beggining
    std::string num2;  // end

    bool operator==(const array& other) const {
        return pidentifier == pidentifier;
    }
};

class DeclarationsNode : public ASTNode {
   public:
    std::vector<array> arrays;
    std::vector<std::string> pidentifiers;

    DeclarationsNode();
    virtual ~DeclarationsNode();

    void print(int indent = 0) const override;
    void addArray(const std::string& pidentifier, const std::string& num1, const std::string& num2) ;
    void addPidentifier(const std::string& pidentifier);
};

}  // namespace ast

#endif  // DECLARATIONS_NODE_HPP
