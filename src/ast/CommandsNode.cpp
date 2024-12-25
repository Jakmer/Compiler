#include "CommandsNode.hpp"
#include <iostream>

namespace ast {

// -- CommandsNode --

CommandsNode::CommandsNode() = default;

void CommandsNode::print(int indent) const {
    std::string pad(indent, ' ');
    std::cout << pad << "CommandsNode\n";
    for (auto& cmd : commands) {
        if (cmd) cmd->print(indent + 2);
    }
}

// -- AssignmentNode --

AssignmentNode::AssignmentNode() = default;

void AssignmentNode::print(int indent) const {
    std::string pad(indent, ' ');
    std::cout << pad << "AssignmentNode\n";
    if (identifier) {
        std::cout << pad << "  identifier:\n";
        identifier->print(indent + 2);
    }
    if (expression) {
        std::cout << pad << "  expression:\n";
        expression->print(indent + 2);
    }
}

// -- IfStatementNode --

IfStatementNode::IfStatementNode() = default;

void IfStatementNode::print(int indent) const {
    std::string pad(indent, ' ');
    std::cout << pad << "IfStatementNode\n";
    if (condition) {
        std::cout << pad << "  condition:\n";
        condition->print(indent + 2);
    }
    if (commands) {
        std::cout << pad << "  commands:\n";
        commands->print(indent + 2);
    }
    if (elseCommands.has_value() && elseCommands.value()) {
        std::cout << pad << "  elseCommands:\n";
        elseCommands.value()->print(indent + 2);
    }
}

// -- WhileStatementNode --

WhileStatementNode::WhileStatementNode() = default;

void WhileStatementNode::print(int indent) const {
    std::string pad(indent, ' ');
    std::cout << pad << "WhileStatementNode\n";
    if (condition) {
        std::cout << pad << "  condition:\n";
        condition->print(indent + 2);
    }
    if (commands) {
        std::cout << pad << "  commands:\n";
        commands->print(indent + 2);
    }
}

// -- RepeatStatementNode --

RepeatStatementNode::RepeatStatementNode() = default;

void RepeatStatementNode::print(int indent) const {
    std::string pad(indent, ' ');
    std::cout << pad << "RepeatStatementNode\n";
    if (commands) {
        std::cout << pad << "  commands:\n";
        commands->print(indent + 2);
    }
    if (condition) {
        std::cout << pad << "  condition:\n";
        condition->print(indent + 2);
    }
}

// -- ForToNode --

ForToNode::ForToNode() = default;

void ForToNode::print(int indent) const {
    std::string pad(indent, ' ');
    std::cout << pad << "ForToNode: " << pidentifier << "\n";
    if (valueFrom) {
        std::cout << pad << "  valueFrom:\n";
        valueFrom->print(indent + 2);
    }
    if (valueTo) {
        std::cout << pad << "  valueTo:\n";
        valueTo->print(indent + 2);
    }
    if (commands) {
        std::cout << pad << "  commands:\n";
        commands->print(indent + 2);
    }
}

// -- ForDowntoNode --

ForDowntoNode::ForDowntoNode() = default;

void ForDowntoNode::print(int indent) const {
    std::string pad(indent, ' ');
    std::cout << pad << "ForDowntoNode: " << pidentifier << "\n";
    if (valueFrom) {
        std::cout << pad << "  valueFrom:\n";
        valueFrom->print(indent + 2);
    }
    if (valueTo) {
        std::cout << pad << "  valueTo:\n";
        valueTo->print(indent + 2);
    }
    if (commands) {
        std::cout << pad << "  commands:\n";
        commands->print(indent + 2);
    }
}

// -- ProcCallNode --

ProcCallNode::ProcCallNode() = default;

void ProcCallNode::print(int indent) const {
    std::string pad(indent, ' ');
    std::cout << pad << "ProcCallNode: " << pidentifier << "\n";
    if (args) {
        std::cout << pad << "  args:\n";
        args->print(indent + 2);
    }
}

// -- ArgsNode --

ArgsNode::ArgsNode() = default;

void ArgsNode::print(int indent) const {
    std::string pad(indent, ' ');
    std::cout << pad << "ArgsNode\n";
    if (!pidentifiers.empty()) {
        std::cout << pad << "  pidentifiers: ";
        for (auto &id : pidentifiers) {
            std::cout << id << " ";
        }
        std::cout << "\n";
    }
}

// -- ReadNode --

ReadNode::ReadNode() = default;

void ReadNode::print(int indent) const {
    std::string pad(indent, ' ');
    std::cout << pad << "ReadNode\n";
    if (identifier) {
        std::cout << pad << "  identifier:\n";
        identifier->print(indent + 2);
    }
}

// -- WriteNode --

WriteNode::WriteNode() = default;

void WriteNode::print(int indent) const {
    std::string pad(indent, ' ');
    std::cout << pad << "WriteNode\n";
    if (value) {
        std::cout << pad << "  value:\n";
        value->print(indent + 2);
    }
}

}  // namespace ast