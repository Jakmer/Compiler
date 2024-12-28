#include "DeclarationsNode.hpp"
#include <iostream>

namespace ast {

DeclarationsNode::DeclarationsNode() {
    setNodeType(DECLARATIONS_NODE);
}

void DeclarationsNode::print(int indent) const {
    std::string pad(indent, ' ');
    std::cout << pad << "DeclarationsNode\n";

    if (!arrays.empty()) {
        std::cout << pad << "  arrays:\n";
        for (const auto& arr : arrays) {
            std::cout << pad << "    array: " << arr.pidentifier << " [" << arr.num1 << ":" << arr.num2 << "]\n";
        }
    }

    if (!pidentifiers.empty()) {
        std::cout << pad << "  pidentifiers: ";
        for (const auto& id : pidentifiers) {
            std::cout << id << " ";
        }
        std::cout << "\n";
    }
}

void DeclarationsNode::addArray(const std::string& pidentifier, int num1, int num2) {
    arrays.push_back({pidentifier, num1, num2});
}

void DeclarationsNode::addPidentifier(const std::string& pidentifier) {
    pidentifiers.push_back(pidentifier);
}

DeclarationsNode::~DeclarationsNode() = default;

}  // namespace ast