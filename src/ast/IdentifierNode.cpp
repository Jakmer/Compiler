#include "IdentifierNode.hpp"
#include <iostream>

namespace ast {

IdentifierNode::IdentifierNode()
    : pidentifier(std::nullopt),
      Tpidentifier(std::nullopt),
      arrayNumIndex(std::nullopt),
      arrayPidentifierIndex(std::nullopt) {
}

void IdentifierNode::print(int indent) const {
    std::string pad(indent, ' ');
    std::cout << pad << "IdentifierNode\n";

    if (pidentifier.has_value()) {
        std::cout << pad << "  pidentifier: " << pidentifier.value() << "\n";
    }

    if (Tpidentifier.has_value()) {
        std::cout << pad << "  Tpidentifier: " << Tpidentifier.value() << "\n";
    }

    if (arrayNumIndex.has_value()) {
        std::cout << pad << "  arrayNumIndex: " << arrayNumIndex.value() << "\n";
    }

    if (arrayPidentifierIndex.has_value()) {
        std::cout << pad << "  arrayPidentifierIndex: " << arrayPidentifierIndex.value() << "\n";
    }
}

IdentifierNode::~IdentifierNode() = default;

}  // namespace ast