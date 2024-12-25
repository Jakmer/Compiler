#include "ProceduresNode.hpp"
#include <iostream>

namespace ast {

ProceduresNode::ProceduresNode()
    : proc_head(nullptr),
      commands(nullptr) {
}

ProceduresNode::~ProceduresNode() {
    delete proc_head;
    if (declarations.has_value()) {
        delete declarations.value();
    }
    delete commands;
}

void ProceduresNode::print(int indent) const {
    std::string pad(indent, ' ');
    std::cout << pad << "ProceduresNode\n";

    if (proc_head) {
        std::cout << pad << "  proc_head:\n";
        proc_head->print(indent + 2);
    }
    if (declarations.has_value()) {
        std::cout << pad << "  declarations:\n";
        declarations.value()->print(indent + 2);
    }
    if (commands) {
        std::cout << pad << "  commands:\n";
        commands->print(indent + 2);
    }
}

ProcHeadNode::~ProcHeadNode() {
    delete args_decl;
}

ProcHeadNode::ProcHeadNode()
    : pidentifier(""),
      args_decl(nullptr) {
}

void ProcHeadNode::print(int indent) const {
    std::string pad(indent, ' ');
    std::cout << pad << "ProcHead: " << pidentifier << "\n";
    if (args_decl) {
        std::cout << pad << "  args_decl:\n";
        args_decl->print(indent + 2);
    }
}

void ArgsDeclNode::print(int indent) const {
    std::string pad(indent, ' ');
    std::cout << pad << "ArgsDeclNode\n";
    std::cout << pad << "  pidentifiers: ";
    for (auto& id : pidentifiers)
        std::cout << id << " ";
    std::cout << "\n";

    std::cout << pad << "  Tpidentifiers: ";
    for (auto& tid : Tpidentifiers)
        std::cout << tid << " ";
    std::cout << "\n";
}

ArgsDeclNode::~ArgsDeclNode() = default;

}  // namespace ast
