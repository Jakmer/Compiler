#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <stack>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "DeclarationsNode.hpp"
#include "ErrorMessages.hpp"

namespace semana {

enum SymbolType {
    MAIN,
    PROCEDURE,
    VARIABLE,
    ARRAY,
    RVALUE,
    ITERATOR,
    ARRAY_ELEMENT,
    PROCEDURE_ARG,
    UNDEFINED  // error
};

struct Symbol {
    SymbolType symbolType;
    std::string name;
    unsigned long address;  // we declare assembly address here (remember about
                            // array address)
    std::variant<std::monostate, std::string, ast::array,
                 std::vector<std::string>>
        value;  // array cant be assigned whole at once so wa may only store
                // symbol with one value at most
    int scope;  // -1 - global, 0 - main, n >= 1 - procedure        !ONLY IN
                // GLOBA CAN WE DEFINE FUNCTIONS
    bool isInitalized;
    bool wasUsedAsFuncArg;
    bool isArrayIndex;

    Symbol() = default;
    Symbol(std::string &name, SymbolType symbolType)
        : name(name),
          symbolType(symbolType),
          isInitalized(false),
          wasUsedAsFuncArg(false),
          isArrayIndex(false) {}
    Symbol(std::string &name, SymbolType symbolType,
           std::variant<std::monostate, std::string, ast::array,
                        std::vector<std::string>>
               value)
        : name(name),
          symbolType(symbolType),
          value(value),
          isInitalized(false),
          wasUsedAsFuncArg(false),
          isArrayIndex(false) {}
};

enum ScopeType { GLOBAL_SCOPE = -1, MAIN_SCOPE, PROCEDURE_SCOPE, NOT_ASSIGNED };

struct ProcCall {
    bool isProcedureCall;
    std::string latestProcedureName;
    int noArg;
};

struct RuntimeParams {
    bool isAssignment;
    bool isForLoopScope;
    bool isProcedureDeclaration;  // if so then assign next declaration as
                                  // already initalized
    ProcCall procCall;
};

std::string toString(const SymbolType &symbolType); 

} // semana
#endif // SYMBOL_HPP
