#include "ErrorMessages.hpp"

namespace semana {
    
std::ostream& operator<<(std::ostream& os, const ErrorType& error) {
    switch (error) {
        case GOOD:
            os << "No errors.";
            break;
        case UNDECLARED_VARIABLE:
            os << "Undeclared variable.";
            break;
        case INCORRECT_ARRAY_RANGE:
            os << "Incorrect array range.";
            break;
        case DUPLICATE_DECLARATION:
            os << "Duplicate declaration.";
            break;
        case UNDECLARED_PROCEDURE:
            os << "Undeclared procedure.";
            break;
        case INCORRECT_ARGUMENTS:
            os << "Incorrect arguments.";
            break;
        case UNDECLARED_ARRAY:
            os << "Undeclared array.";
            break;
        case UNDECLARED_MAIN:
            os << "Undeclared main function.";
            break;
        case UNDECLARED_GLOBAL:
            os << "No global scope.";
            break;
        case TYPE_MISMATCH:
            os << "Type mismatch.";
            break;
        case RECURSIVE_CALL:
            os << "Recursive procedure call.";
            break;
        case UNINITIALIZED_VARIABLE:
            os << "Uninitialized variable.";
            break;
        case UNINITIALIZED_ARRAY:
            os << "Uninitialized array.";
            break;
        case REDEFINTION:
            os << "Redefinition of symbol.";
            break;
        case BAD_SCOPE:
            os << "Bad scope.";
            break;
        default:
            os << "Unknown error type.";
            break;
    }
    return os;
}
}
