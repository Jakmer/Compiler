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
    ARRAY_ELEMENT,
    UNDEFINED   // error
};

struct Symbol {
    SymbolType symbolType;
    std::string name;
    int address;    // we declare assembly address here (remember about array address)
    std::variant<std::monostate ,std::string, ast::array> value;   // array cant be assigned whole at once so wa may only store symbol with one value at most
    int scope;  // -1 - global, 0 - main, n >= 1 - procedure        !ONLY IN GLOBA CAN WE DEFINE FUNCTIONS
    bool isInitalized;

    Symbol() = default;
    Symbol(std::string &name, SymbolType symbolType) : name(name), symbolType(symbolType), isInitalized(false){}
    Symbol(std::string &name, SymbolType symbolType,     std::variant<std::monostate ,std::string, ast::array> value) : name(name), symbolType(symbolType), value(value), isInitalized(false){}
};

enum ScopeType {
    GLOBAL_SCOPE = -1,
    MAIN_SCOPE,
    PROCEDURE_SCOPE,
    NOT_ASSIGNED
};

inline std::string toString(const SymbolType &symbolType)
{
    switch (symbolType) {
        case SymbolType::VARIABLE:
            return "Variable";
        case SymbolType::PROCEDURE:
            return "Procedure";
        case SymbolType::ARRAY:
            return "Array";
        case SymbolType::RVALUE:
            return "Rvalue";
        case SymbolType::ITERATOR:
            return "Iterator";
        case SymbolType::ARRAY_ELEMENT:
            return "Array element";
        default:
            throw std::invalid_argument("Unkonw symbol type");
    }
}

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

        auto validatationMessage = validateDeclaration(symbol);
        if(validatationMessage.errorType != GOOD)
            return validatationMessage;

        // TODO assign address here
        symbol.address = 0;
        
        auto symbolUniqueName = getSymbolUniqeName(symbol);
        symbols[symbolUniqueName] = symbol;
        return ValidationMessage(GOOD, "");
    }

    void printSymbols() {};

    ValidationMessage validateSymbol(Symbol& symbol, bool isAssignment){
        auto symbolUniqueName = getSymbolUniqeName(symbol);

        auto mapSymbol = symbols.find(symbolUniqueName);
        if(mapSymbol == symbols.end())
            return ValidationMessage(UNDECLARED_VARIABLE, symbol.name);

        auto declaredSymbol = mapSymbol->second;
    
        switch (symbol.symbolType) {
            case MAIN: {
                break;
            }
            case PROCEDURE: {
                if(declaredSymbol.symbolType != symbol.symbolType)
                    return ValidationMessage(TYPE_MISMATCH, "Expected: "+toString(declaredSymbol.symbolType)+ " Received: "+toString(symbol.symbolType));
                break;
            }
            case VARIABLE: {
                if(declaredSymbol.symbolType == ARRAY) 
                    return ValidationMessage(TYPE_MISMATCH, "Expected: "+toString(declaredSymbol.symbolType)+ " Received: "+toString(symbol.symbolType));
                break;
            }
            case ARRAY: {
                if(!isAssignment && declaredSymbol.symbolType != ARRAY) 
                    return ValidationMessage(TYPE_MISMATCH, "Expected: "+toString(declaredSymbol.symbolType)+ " Received: "+toString(symbol.symbolType));
                if(isAssignment)
                {
                    // we can treat it like variable in this case
                }
                break;
            }
            case RVALUE: {
                if(declaredSymbol.symbolType == ARRAY) 
                    return ValidationMessage(TYPE_MISMATCH, "Expected: "+toString(declaredSymbol.symbolType)+ " Received: "+toString(symbol.symbolType));
                break;
            }
            case ITERATOR: {
                if(declaredSymbol.symbolType == ARRAY) 
                    return ValidationMessage(TYPE_MISMATCH, "Expected: "+toString(declaredSymbol.symbolType)+ " Received: "+toString(symbol.symbolType));
                break;
            }
            case UNDEFINED: {
                throw std::runtime_error("Undefined symbol type!");
            }
        }

        return ValidationMessage(GOOD, "");
    }
    
    private:

    ValidationMessage validateDeclaration(Symbol& symbol){
        auto symbolUniqueName = getSymbolUniqeName(symbol);

        return ValidationMessage(GOOD, "");
    }

    std::string getSymbolUniqeName(Symbol &symbol){
        std::string uniqueName;

        if(symbol.symbolType == PROCEDURE)
            uniqueName = "procedure" + symbol.name;
        else
            uniqueName = std::to_string(scopes.top()) + symbol.name;

        return uniqueName;
    }

    std::stack<int> scopes; // only for scope checking
    std::unordered_map<std::string, Symbol> symbols;    // name and symbol
    int noProcedures = 0;
};

}  // namespace semana

#endif // SYMBOL_TABLE_HPP
