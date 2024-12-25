#include "MainNode.hpp"
#include <iostream>

namespace ast {

MainNode::MainNode()
    : declarations(nullptr), commands(nullptr) {
}

void MainNode::print(int indent) const {
    std::string pad(indent, ' ');
    std::cout << pad << "MainNode\n";

    if (declarations) {
        std::cout << pad << "  declarations:\n";
        declarations->print(indent + 2);
    }

    if (commands) {
        std::cout << pad << "  commands:\n";
        commands->print(indent + 2);
    }
}

}  // namespace ast
