#ifndef AST_NODE_HPP
#define AST_NODE_HPP

#include <iostream>
#include <memory>
#include <string>
#include <vector>

//namespace ast {

class ASTNode {
public:
    virtual ~ASTNode() = default;

    virtual void print(int indent = 0) const = 0;

protected:
    void printIndent(int indent) const;
};

//}  // namespace ast

#endif // ASTNODE_HPP
