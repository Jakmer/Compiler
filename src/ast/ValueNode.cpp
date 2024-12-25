#include "ValueNode.hpp"
#include <iostream>

namespace ast {

ValueNode::ValueNode()
    : num(std::nullopt),
      identifier(std::nullopt) {
}

void ValueNode::print(int indent) const {
    std::string pad(indent, ' ');
    std::cout << pad << "ValueNode\n";

    if (num.has_value()) {
        std::cout << pad << "  num: " << num.value() << "\n";
    }

    if (identifier.has_value() && identifier.value()) {
        std::cout << pad << "  identifier:\n";
        identifier.value()->print(indent + 2);
    }
}

}  // namespace ast