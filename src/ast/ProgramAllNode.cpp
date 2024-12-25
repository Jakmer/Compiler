#include "ProgramAllNode.hpp"
#include <iostream>

namespace ast {

ProgramAllNode::ProgramAllNode()
    : main(nullptr) {
}

ProgramAllNode::~ProgramAllNode() {
    for (auto& proc : procedures) {
        delete proc;
    }
    delete main;
}

void ProgramAllNode::print(int indent) const {
    std::string pad(indent, ' ');
    std::cout << pad << "ProgramAllNode\n";

    if (!procedures.empty()) {
        std::cout << pad << "  procedures:\n";
        for (auto& proc : procedures) {
            if (proc) {
                proc->print(indent + 2);
            }
        }
    }

    if (main) {
        std::cout << pad << "  main:\n";
        main->print(indent + 2);
    }
}

}  // namespace ast
