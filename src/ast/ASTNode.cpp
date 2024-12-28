#include "ASTNode.hpp"

void ASTNode::printIndent(int indent) const {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
}

void ASTNode::setNodeType(NodeType type) {
    nodeType = type;
}

NodeType ASTNode::getNodeType() const {
    return nodeType;
}

void ASTNode::setPosition(int &line, int &column) {
    pos.line = line;
    pos.column = column;
}

position ASTNode::getPosition() const {
    return pos;
}