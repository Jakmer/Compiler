#include "ExpressionNode.hpp"
#include <iostream>

namespace ast {

ExpressionNode::ExpressionNode()
    : value1(nullptr),
      value2(std::nullopt),
      mathOperation(std::nullopt) {
}

void ExpressionNode::print(int indent) const {
    std::string pad(indent, ' ');
    std::cout << pad << "ExpressionNode\n";

    // Print value1
    if (value1) {
        std::cout << pad << "  value1:\n";
        value1->print(indent + 2);
    }

    // Print mathOperation if it exists
    if (mathOperation.has_value()) {
        std::cout << pad << "  mathOperation: ";
        switch (mathOperation.value()) {
            case PLUS:      std::cout << "PLUS";      break;
            case SUBSTRACT: std::cout << "SUBSTRACT"; break;
            case MULTIPLY:  std::cout << "MULTIPLY";  break;
            case DIVIDE:    std::cout << "DIVIDE";    break;
            case MOD:       std::cout << "MOD";       break;
        }
        std::cout << "\n";
    }

    // Print value2 if it exists
    if (value2.has_value() && value2.value()) {
        std::cout << pad << "  value2:\n";
        value2.value()->print(indent + 2);
    }
}

}  // namespace ast