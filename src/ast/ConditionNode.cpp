#include "ConditionNode.hpp"
#include <iostream>

namespace ast {

ConditionNode::ConditionNode()
    : value1(nullptr),
      value2(nullptr),
      relation(EQ) {
}

void ConditionNode::print(int indent) const {
    std::string pad(indent, ' ');
    std::cout << pad << "ConditionNode\n";

    // Print the relation operation
    std::cout << pad << "  relation: ";
    switch (relation) {
        case EQ:        std::cout << "EQ";        break;
        case NEQ:       std::cout << "NEQ";       break;
        case LT:        std::cout << "LT";        break;
        case LE:        std::cout << "LE";        break;
        case GT:        std::cout << "GT";        break;
        case GE:        std::cout << "GE";        break;
    }
    std::cout << "\n";

    // Print value1
    if (value1) {
        std::cout << pad << "  value1:\n";
        value1->print(indent + 2);
    }

    // Print value2
    if (value2) {
        std::cout << pad << "  value2:\n";
        value2->print(indent + 2);
    }
}

ConditionNode::~ConditionNode() {
    delete value1;
    delete value2;
}

}  // namespace ast
