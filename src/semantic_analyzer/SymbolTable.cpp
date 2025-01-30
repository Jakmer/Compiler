#include "SymbolTable.hpp"

namespace semana{

SymbolTable::SymbolTable() : noProcedures(0), address(1){};  // address start from 5 because 0 is reserved for accumulator

ValidationMessage SymbolTable::openScope(ScopeType scopeType) {
    switch (scopeType) {
        case GLOBAL_SCOPE: {
            if (!scopes.empty())
                // Always open global scope before adding real scopes
                throw std::runtime_error("No GlobalScope defined!");
            scopes.push(-1);  // -1 is reserved for global scope only
            break;
        }
        case MAIN_SCOPE: {
            if (scopes.top() != GLOBAL_SCOPE)
                return ValidationMessage(
                    BAD_SCOPE,
                    "Function can be declared only in global scope");
            scopes.push(0);  // 0 is reserved for main scope only
            break;
        }
        case PROCEDURE_SCOPE: {
            if (scopes.top() != GLOBAL_SCOPE)
                return ValidationMessage(
                    BAD_SCOPE,
                    "Function can be declared only in global scope");
            scopes.push(++noProcedures);
            break;
        }
        default:
            throw std::runtime_error("Undefined Scope");
    }

    return ValidationMessage(GOOD, "");
};

ValidationMessage SymbolTable::closeScope() {
    if (scopes.empty())
        throw std::runtime_error("Cannot close empty scope!");

    scopes.pop();

    if (scopes.top() != GLOBAL_SCOPE)
        return ValidationMessage(
            BAD_SCOPE,
            "After closing scope we should be back in global scope!");

    return ValidationMessage(GOOD, "");
}

ValidationMessage SymbolTable::addSymbol(Symbol &symbol, RuntimeParams &runtimeParams) {
    symbol.scope = scopes.top();

    auto validatationMessage = validateDeclaration(symbol, runtimeParams);
    if (validatationMessage.errorType != GOOD) return validatationMessage;

    auto symbolUniqueName = getSymbolUniqeName(symbol);

    if (runtimeParams.isProcedureDeclaration) {
        symbol.isInitalized =
            true;  // verification will be done on call function side

        auto &procedureSymbol = getLatestProcedureName(
            runtimeParams.procCall.latestProcedureName);

        if (!std::holds_alternative<std::vector<std::string>>(
                procedureSymbol.value)) {
            procedureSymbol.value = std::vector<std::string>();
        }
        auto &vec =
            std::get<std::vector<std::string>>(procedureSymbol.value);
        vec.insert(vec.begin(), symbolUniqueName);  // change to deque
    }

    assignAddress(symbol);
    symbols[symbolUniqueName] = symbol;

    return ValidationMessage(GOOD, "");
}

ValidationMessage SymbolTable::validateSymbol(Symbol &symbol,
                                    semana::RuntimeParams &runtimeParams) {
    auto symbolUniqueName = getSymbolUniqeName(symbol);

    auto mapSymbol = symbols.find(symbolUniqueName);
    if (mapSymbol == symbols.end())
        return ValidationMessage(UNDECLARED_VARIABLE, symbol.name);

    auto &declaredSymbol = mapSymbol->second;
    if (declaredSymbol.symbolType == ITERATOR) symbol.symbolType = ITERATOR;

    switch (symbol.symbolType) {
        case MAIN: {
            break;
        }
        case PROCEDURE: {
            if (declaredSymbol.symbolType != symbol.symbolType)
                return ValidationMessage(
                    TYPE_MISMATCH,
                    "Expected: " + toString(declaredSymbol.symbolType) +
                        " Received: " + toString(symbol.symbolType));

            if (declaredSymbol.scope == scopes.top())
                return ValidationMessage(RECURSIVE_CALL, symbol.name);

            // validate if call is correct (if arguemnts are the same like
            // in declaration)
            break;
        }
        case VARIABLE: {
            if (declaredSymbol.symbolType == ARRAY)
                return ValidationMessage(
                    TYPE_MISMATCH,
                    "Expected: " + toString(declaredSymbol.symbolType) +
                        " Received: " + toString(symbol.symbolType));
            if (runtimeParams.isAssignment ||
                runtimeParams.procCall.isProcedureCall)
                declaredSymbol.isInitalized =
                    true;  // second condition is dangerous - variable can
                            // stay unitialzed after call
            if (!declaredSymbol.isInitalized)
                return ValidationMessage(UNINITIALIZED_VARIABLE,
                                            symbol.name);
            break;
        }
        case ARRAY: {
            if (!runtimeParams.isAssignment &&
                declaredSymbol.symbolType != ARRAY)
                return ValidationMessage(
                    TYPE_MISMATCH,
                    "Expected: " + toString(declaredSymbol.symbolType) +
                        " Received: " + toString(symbol.symbolType));
            if (runtimeParams.isAssignment)
                declaredSymbol.isInitalized = true;
            if (runtimeParams.isAssignment) {
                // we can treat it like variable in this case
            }
            break;
        }
        case RVALUE: {
            if (declaredSymbol.symbolType == ARRAY)
                return ValidationMessage(
                    TYPE_MISMATCH,
                    "Expected: " + toString(declaredSymbol.symbolType) +
                        " Received: " + toString(symbol.symbolType));
            break;
        }
        case ITERATOR: {
            if (declaredSymbol.symbolType == ARRAY)
                return ValidationMessage(
                    TYPE_MISMATCH,
                    "Expected: " + toString(declaredSymbol.symbolType) +
                        " Received: " + toString(symbol.symbolType));
            if (declaredSymbol.symbolType == VARIABLE) {
                declaredSymbol.symbolType = VARIABLE;
                declaredSymbol.isInitalized = true;
            }
            if (!symbol.isArrayIndex && runtimeParams.isAssignment)
                return ValidationMessage(ITERATOR_MODIFICATION,
                                            symbol.name);
            break;
        }
        case ARRAY_ELEMENT: {
            if (runtimeParams.isAssignment)
                declaredSymbol.isInitalized = true;
            if (declaredSymbol.symbolType != ARRAY)
                return ValidationMessage(
                    TYPE_MISMATCH,
                    "Expected: " + toString(declaredSymbol.symbolType) +
                        " Received: " + toString(symbol.symbolType));

            break;
        }
        case PROCEDURE_ARG: {
            auto procedureSymbol = getLatestProcedureName(
                runtimeParams.procCall.latestProcedureName);
            // check if procedure has arguments
            if (!std::holds_alternative<std::vector<std::string>>(
                    procedureSymbol.value))
                throw std::runtime_error("Procedure has no arguments!");

            auto procedureArgs =
                std::get<std::vector<std::string>>(procedureSymbol.value);
            auto originalArgName =
                procedureArgs.at(runtimeParams.procCall.noArg);
            auto originalArgSymbol = symbols[originalArgName];
            auto expectedType = originalArgSymbol.symbolType;
            auto receivedType = declaredSymbol.symbolType;

            if (expectedType != receivedType)
                return ValidationMessage(
                    TYPE_MISMATCH,
                    "Expected: " + toString(expectedType) +
                        " Received: " + toString(receivedType));

            declaredSymbol.isInitalized = true;

            break;
        }
        case UNDEFINED: {
            throw std::runtime_error("Undefined symbol type!");
        }
    }

    return ValidationMessage(GOOD, "");
}


ValidationMessage SymbolTable::validateDeclaration(Symbol &symbol,
                                        RuntimeParams &runtimeParams) {
    auto symbolUniqueName = getSymbolUniqeName(symbol);

    if (symbol.symbolType != RVALUE &&
        symbols.find(symbolUniqueName) != symbols.end())
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


std::string SymbolTable::getSymbolUniqeName(Symbol &symbol, int scope) {
    std::string uniqueName;

    if (symbol.symbolType == PROCEDURE)
        uniqueName = "procedure" + symbol.name;
    else
        uniqueName = std::to_string(scopes.top()) + symbol.name;

    return uniqueName;
}


Symbol &SymbolTable::getLatestProcedureName(std::string &name) {
    std::string prefix = "procedure";
    std::string procedureName = prefix + name;

    return symbols[procedureName];
}


void SymbolTable::assignAddress(Symbol &symbol) {
    if (symbol.symbolType == PROCEDURE) {
        symbol.address = 0;  // TODO: make it unaddressable - its address is
                                // define with assembler operations number
        return;
    }

    symbol.address = address;

    if (symbol.symbolType == ARRAY &&
        std::holds_alternative<ast::array>(symbol.value)) {
        std::string beginning = std::get<ast::array>(symbol.value).num1;
        std::string end = std::get<ast::array>(symbol.value).num2;
        unsigned long size = std::stoul(end) - std::stoul(beginning) + 1;
        address += size;
    } else {
        address++;
    }
}

int SymbolTable::getScopeByProcName(std::string &name){
    if(name=="main")
        return 0;
    auto symbol = getLatestProcedureName(name);
    return symbol.scope;
}

Symbol SymbolTable::getSymbolByName(std::string &name, int &scope){
    std::string symbolUniqueName = std::to_string(scope) + name;
    return symbols[symbolUniqueName];
}

unsigned long SymbolTable::getLastUsedAddr(){
    return address;
}

}

