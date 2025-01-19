#ifndef SYMBOL_TABLE_HPP 
#define SYMBOL_TABLE_HPP

#include <iostream>
#include <variant>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <stack>

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
    UNDEFINED   // error
};

struct Symbol {
    SymbolType symbolType;
    std::string name;
    int address;    // we declare assembly address here (remember about array address)
    std::variant<std::monostate ,std::string, ast::array> value;   // array cant be assigned whole at once so wa may only store symbol with one value at most
    int scope;  // -1 - global, 0 - main, n >= 1 - procedure        !ONLY IN GLOBA CAN WE DEFINE FUNCTIONS

    Symbol() = default;
    Symbol(std::string &name, SymbolType symbolType) : name(name), symbolType(symbolType){}
    Symbol(std::string &name, SymbolType symbolType,     std::variant<std::monostate ,std::string, ast::array> value) : name(name), symbolType(symbolType), value(value){}
};

enum ScopeType {
    GLOBAL_SCOPE = -1,
    MAIN_SCOPE,
    PROCEDURE_SCOPE,
    NOT_ASSIGNED
};

class SymbolTable {
    public:
    SymbolTable() = default;
    ValidationMessage openScope(ScopeType scopeType) {

        switch (scopeType) {
            case GLOBAL_SCOPE: {
                if(!scopes.empty())
                    // Always open global scope before adding real scopes
                    throw std::runtime_error("No GlobalScope defined!");
                scopes.push(-1);    // -1 is reserved for global scope only
                break;
            }
            case MAIN_SCOPE: {
                if(scopes.top() != GLOBAL_SCOPE)
                    return ValidationMessage(BAD_SCOPE, "Function can be declared only in global scope");
                scopes.push(0);     // 0 is reserved for main scope only
                break;
            }
            case PROCEDURE_SCOPE: {
                if(scopes.top() != GLOBAL_SCOPE)
                    return ValidationMessage(BAD_SCOPE, "Function can be declared only in global scope");
                scopes.push(++noProcedures);
                break;
            }
            default:
                throw std::runtime_error("Undefined Scope");
        }

        return ValidationMessage(GOOD, "");
    };

    ValidationMessage closeScope() {
       if(scopes.empty()) 
            throw std::runtime_error("Cannot close empty scope!");

        scopes.pop();

        if(scopes.top() != GLOBAL_SCOPE)
            return ValidationMessage(BAD_SCOPE, "After closing scope we should be back in global scope!");

        return ValidationMessage(GOOD, "");
    }

    ValidationMessage addSymbol(Symbol &symbol) {
        symbol.scope = scopes.top();

        auto validatationMessage = validateSymbol(symbol);
        if(validatationMessage.errorType != GOOD)
            return validatationMessage;

        // TODO assign address here
        symbol.address = 0;
        
        auto symbolUniqueName = getSymbolUniqeName(symbol.name);
        symbols[symbolUniqueName] = symbol;
        return ValidationMessage(GOOD, "");
    }

    void printSymbols() {};

    private:

    ValidationMessage validateSymbol(Symbol& symbol){
        auto symbolUniqueName = getSymbolUniqeName(symbol.name);

        return ValidationMessage(GOOD, "");
    }
    
    std::string getSymbolUniqeName(std::string &name){
        return std::to_string(scopes.top()) + name;
    }

    std::stack<int> scopes; // only for scope checking
    std::unordered_map<std::string, Symbol> symbols;    // name and symbol
    int noProcedures = 0;
};

}  // namespace semana

#endif // SYMBOL_TABLE_HPP
