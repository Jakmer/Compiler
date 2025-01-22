#ifndef SYMBOL_TABLE_HPP 
#define SYMBOL_TABLE_HPP

#include <iostream>
#include <optional>
#include <variant>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <stack>
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
    UNDEFINED   // error
};

struct Symbol {
    SymbolType symbolType;
    std::string name;
    int address;    // we declare assembly address here (remember about array address)
    std::variant<std::monostate ,std::string, ast::array, std::vector<std::string>> value;   // array cant be assigned whole at once so wa may only store symbol with one value at most
    int scope;  // -1 - global, 0 - main, n >= 1 - procedure        !ONLY IN GLOBA CAN WE DEFINE FUNCTIONS
    bool isInitalized;
    bool wasUsedAsFuncArg;
    bool isArrayIndex;

    Symbol() = default;
    Symbol(std::string &name, SymbolType symbolType) : name(name), symbolType(symbolType), isInitalized(false), wasUsedAsFuncArg(false), isArrayIndex(false){}
    Symbol(std::string &name, SymbolType symbolType,     std::variant<std::monostate ,std::string, ast::array, std::vector<std::string>> value) : name(name), symbolType(symbolType), value(value), isInitalized(false), wasUsedAsFuncArg(false), isArrayIndex(false){}
};

enum ScopeType {
    GLOBAL_SCOPE = -1,
    MAIN_SCOPE,
    PROCEDURE_SCOPE,
    NOT_ASSIGNED
};

struct ProcCall{
    bool isProcedureCall;
    std::string latestProcedureName;
    int noArg;
};

struct RuntimeParams{
    bool isAssignment;
    bool isForLoopScope;
    bool isProcedureDeclaration; // if so then assign next declaration as already initalized
    ProcCall procCall;
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
        case SymbolType::PROCEDURE_ARG:
            return "Procedure argument";
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

    ValidationMessage addSymbol(Symbol &symbol, RuntimeParams &runtimeParams) {
        symbol.scope = scopes.top();

        auto validatationMessage = validateDeclaration(symbol, runtimeParams);
        if(validatationMessage.errorType != GOOD)
            return validatationMessage;

        // TODO assign address here
        symbol.address = 0;
        auto symbolUniqueName = getSymbolUniqeName(symbol);

        if(runtimeParams.isProcedureDeclaration)
        {
            symbol.isInitalized = true; // verification will be done on call function side

            auto &procedureSymbol = getLatestProcedureName(runtimeParams.procCall.latestProcedureName);
        
            if (!std::holds_alternative<std::vector<std::string>>(procedureSymbol.value)) {
                procedureSymbol.value = std::vector<std::string>();
            }
            auto &vec = std::get<std::vector<std::string>>(procedureSymbol.value);  
            vec.insert(vec.begin(), symbolUniqueName);  // change to deque
        }

        symbols[symbolUniqueName] = symbol;
        
        return ValidationMessage(GOOD, "");
    }

    void printSymbols() {};

    ValidationMessage validateSymbol(Symbol& symbol, semana::RuntimeParams &runtimeParams){
        auto symbolUniqueName = getSymbolUniqeName(symbol);

        auto mapSymbol = symbols.find(symbolUniqueName);
        if(mapSymbol == symbols.end())
            return ValidationMessage(UNDECLARED_VARIABLE, symbol.name);

        auto &declaredSymbol = mapSymbol->second;
        if(declaredSymbol.symbolType==ITERATOR)
            symbol.symbolType = ITERATOR;
    
        switch (symbol.symbolType) {
            case MAIN: {
                break;
            }
            case PROCEDURE: {
                if(declaredSymbol.symbolType != symbol.symbolType)
                    return ValidationMessage(TYPE_MISMATCH, "Expected: "+toString(declaredSymbol.symbolType)+ " Received: "+toString(symbol.symbolType));

                if(declaredSymbol.scope == scopes.top())
                    return ValidationMessage(RECURSIVE_CALL, symbol.name);

                // validate if call is correct (if arguemnts are the same like in declaration)
                break;
            }
            case VARIABLE: {
                if(declaredSymbol.symbolType == ARRAY) 
                    return ValidationMessage(TYPE_MISMATCH, "Expected: "+toString(declaredSymbol.symbolType)+ " Received: "+toString(symbol.symbolType));
                if(runtimeParams.isAssignment || runtimeParams.procCall.isProcedureCall)
                    declaredSymbol.isInitalized = true;     // second condition is dangerous - variable can stay unitialzed after call
                if(!declaredSymbol.isInitalized)
                    return ValidationMessage(UNINITIALIZED_VARIABLE, symbol.name);
                break;
            }
            case ARRAY: {
                if(!runtimeParams.isAssignment && declaredSymbol.symbolType != ARRAY) 
                    return ValidationMessage(TYPE_MISMATCH, "Expected: "+toString(declaredSymbol.symbolType)+ " Received: "+toString(symbol.symbolType));
                if(runtimeParams.isAssignment)
                    declaredSymbol.isInitalized = true;
                if(runtimeParams.isAssignment)
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
                if(declaredSymbol.symbolType == VARIABLE)
                {
                    declaredSymbol.symbolType = VARIABLE;
                    declaredSymbol.isInitalized = true;
                }
                if(!symbol.isArrayIndex && runtimeParams.isAssignment)
                    return ValidationMessage(ITERATOR_MODIFICATION, symbol.name);
                break;
            }
            case ARRAY_ELEMENT: {
                if(runtimeParams.isAssignment)
                    declaredSymbol.isInitalized = true;
                if(declaredSymbol.symbolType != ARRAY)
                    return ValidationMessage(TYPE_MISMATCH, "Expected: "+toString(declaredSymbol.symbolType)+ " Received: "+toString(symbol.symbolType));

                break;
            }
            case PROCEDURE_ARG: {
                auto procedureSymbol = getLatestProcedureName(runtimeParams.procCall.latestProcedureName);
                // check if procedure has arguments
                if(!std::holds_alternative<std::vector<std::string>>(procedureSymbol.value))
                    throw std::runtime_error("Procedure has no arguments!");

                auto procedureArgs = std::get<std::vector<std::string>>(procedureSymbol.value);
                auto originalArgName = procedureArgs.at(runtimeParams.procCall.noArg);
                auto originalArgSymbol = symbols[originalArgName];
                auto expectedType = originalArgSymbol.symbolType;
                auto receivedType = declaredSymbol.symbolType;

                if(expectedType != receivedType)
                    return ValidationMessage(TYPE_MISMATCH, "Expected: "+toString(expectedType)+ " Received: "+toString(receivedType));

                declaredSymbol.isInitalized = true;

                break;
            }
            case UNDEFINED: {
                throw std::runtime_error("Undefined symbol type!");
            }
        }

        return ValidationMessage(GOOD, "");
    }
    
    private:

    ValidationMessage validateDeclaration(Symbol& symbol, RuntimeParams &runtimeParams){
        auto symbolUniqueName = getSymbolUniqeName(symbol);

        if(symbol.symbolType!= RVALUE && symbols.find(symbolUniqueName) != symbols.end())
            return ValidationMessage(REDEFINTION, symbol.name);

        switch (symbol.symbolType) {
            case MAIN: {
                break;
            }
            case PROCEDURE: {
                break;
            }
            case VARIABLE: {
                break;
            }
            case ARRAY: {
                break;
            }
            case RVALUE: {
                break;
            }
            case ITERATOR: {
                break;
            }
            case ARRAY_ELEMENT: {
                break;
            }
            case PROCEDURE_ARG: {
                break;
            }
            case UNDEFINED: {
                throw std::runtime_error("Undefined symbol type!");
            }
        }

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

    Symbol& getLatestProcedureName(std::string &name)
    {
        std::string prefix = "procedure";
        std::string procedureName = prefix + name;
        
        return symbols[procedureName];
    }

    std::stack<int> scopes; // only for scope checking
    std::unordered_map<std::string, Symbol> symbols;    // name and symbol
    int noProcedures = 0;
};

}  // namespace semana

#endif // SYMBOL_TABLE_HPP
