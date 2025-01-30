#ifndef COMMAND_HPP
#define COMMAND_HPP

namespace codegen {
enum Command {
    READ,
    WRITE,
    ASSIGN,
    CONDITION,
    IF,
    IF_ELSE,
    WHILE,
    REPEAT,
    FOR_TO,
    FORM_FROM,
    PROC_CALL,
    UNDEFINED
};
}

#endif  // !COMMAND_HPP
