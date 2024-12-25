#include "CommandsNode.hpp"
#include <iostream>

namespace ast {

// -- CommandsNode --

CommandsNode::CommandsNode() = default;

CommandsNode::~CommandsNode() {
    for (auto& cmd : commands) {
        delete cmd;
    }
}

void CommandsNode::print(int indent) const {
    std::string pad(indent, ' ');
    std::cout << pad << "CommandsNode\n";
    for (auto& cmd : commands) {
        if (cmd) cmd->print(indent + 2);
    }
}

// -- AssignmentNode --

AssignmentNode::AssignmentNode() = default;

AssignmentNode::~AssignmentNode() {
    delete identifier;
    delete expression;
}

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

IfStatementNode::~IfStatementNode() {
    delete condition;
    delete commands;
    if (elseCommands.has_value()) {
        delete elseCommands.value();
    }
}

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

WhileStatementNode::~WhileStatementNode() {
    delete condition;
    delete commands;
}

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

RepeatStatementNode::~RepeatStatementNode() {
    delete condition;
    delete commands;
}

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

ForToNode::~ForToNode() {
    delete valueFrom;
    delete valueTo;
    delete commands;
}

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

ForDowntoNode::~ForDowntoNode() {
    delete valueFrom;
    delete valueTo;
    delete commands;
}

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

ProcCallNode::~ProcCallNode() {
    delete args;
}

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

ArgsNode::~ArgsNode() = default;

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

ReadNode::~ReadNode() {
    delete identifier;
}

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

WriteNode::~WriteNode() {
    delete value;
}

void WriteNode::print(int indent) const {
    std::string pad(indent, ' ');
    std::cout << pad << "WriteNode\n";
    if (value) {
        std::cout << pad << "  value:\n";
        value->print(indent + 2);
    }
}

}  // namespace ast