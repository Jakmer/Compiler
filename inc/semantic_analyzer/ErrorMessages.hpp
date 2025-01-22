#ifndef ERROR_MESSAGES_HPP
#define ERROR_MESSAGES_HPP

#include <string>
#include <ostream>

namespace semana {

enum ExitCode{
    SUCCESS = 0,
    SEMANTIC_ERROR = 1,
    LEXICAL_ERROR = 2,
    SYNTAX_ERROR = 3,
    INTERNAL_ERROR = 4,
    UNDEFINED_ERROR = 5
};

enum ErrorType { GOOD,
    UNDECLARED_VARIABLE,
    INCORRECT_ARRAY_RANGE,
    DUPLICATE_DECLARATION,
    UNDECLARED_PROCEDURE,
    INCORRECT_ARGUMENTS,
    UNDECLARED_ARRAY,
    UNDECLARED_MAIN,
    UNDECLARED_GLOBAL,
    TYPE_MISMATCH,
    RECURSIVE_CALL,
    UNINITIALIZED_VARIABLE,
    UNINITIALIZED_ARRAY,
    REDEFINTION,
    BAD_SCOPE,
    ITERATOR_MODIFICATION
    /*ITERATOR // when receive ITERATOR, we should check on AST whether is in RANGE*/
};

struct ValidationMessage {
    ErrorType errorType;
    std::string content;
    
    ValidationMessage(ErrorType errorType, std::string content) : errorType(errorType), content(content) {}
};

std::ostream& operator<<(std::ostream& os, const ErrorType& error);

}

#endif // ERROR_MESSAGES_HPP
