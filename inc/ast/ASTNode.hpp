#ifndef AST_NODE_HPP
#define AST_NODE_HPP

#include <iostream>
#include <memory>
#include <string>
#include <vector>

enum NodeType {
    PROGRAM_ALL_NODE,
    PROCEDURES_NODE,
    MAIN_NODE,
    COMMANDS_NODE,
    ASSIGNMENT_NODE,
    IF_STATEMENT_NODE,
    WHILE_STATEMENT_NODE,
    REPEAT_STATEMENT_NODE,
    FOR_TO_NODE,
    FOR_DOWNTO_NODE,
    PROC_CALL_NODE,
    READ_NODE,
    WRITE_NODE,
    PROC_HEAD_NODE,
    DECLARATIONS_NODE,
    ARGS_DECL_NODE,
    ARGS_NODE,
    EXPRESSION_NODE,
    CONDITION_NODE,
    VALUE_NODE,
    IDENTIFIER_NODE
};

struct position {
    int line;
    int column;
};

class ASTNode {
public:
    virtual ~ASTNode() = default;

    virtual void print(int indent = 0) const = 0;

    void setNodeType(NodeType type);
    void setPosition(int &line, int &column);
    position getPosition() const;
    NodeType getNodeType() const;

protected:
    void printIndent(int indent) const;

    NodeType nodeType;
    position pos;

};

#endif // ASTNODE_HPP
