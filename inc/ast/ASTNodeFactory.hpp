#ifndef AST_NODE_FACTORY_HPP
#define AST_NODE_FACTORY_HPP

#include "AST.hpp"

namespace ast {

class ASTNodeFactory {
public:
    static ASTNode* createNode(NodeType type);
    template <typename T>
    static T* castNode(ASTNode* node) {
        return dynamic_cast<T*>(node);
    }
};

}  // namespace ast

#endif  // AST_NODE_FACTORY_HPP
