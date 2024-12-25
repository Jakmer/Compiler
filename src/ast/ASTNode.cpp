#include "ASTNode.hpp"

//using namespace ast;

void ASTNode::printIndent(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
}
