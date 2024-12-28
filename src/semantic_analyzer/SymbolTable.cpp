#include "SymbolTable.hpp"

#include <optional>

namespace semana {

void SymbolTable::insert(const Symbol& symbol) {
    if (scopeStack.empty()) {
        throw std::runtime_error("No open scope to insert symbol");
    }

    auto validateMsg = validateDeclaration(symbol);
    if (validateMsg.errorType != ErrorType::GOOD) {
        // add error
    } else {
        scopeStack.top().symbols.push_back(symbol);
    }
}

void SymbolTable::openScope(std::string& name, ScopeType scopeType) {
    Scope newScope;
    newScope.name = name;
    newScope.scopeType = scopeType;
    scopeStack.push(newScope);
}

void SymbolTable::closeScope() {
    if (!scopeStack.empty()) {
        scopeStack.pop();
    } else {
        throw std::runtime_error("No open scope to close");
    }
}

// in message return symbol position
ValidationMessage SymbolTable::validateDeclaration(const Symbol& symbol) {
    if (scopeStack.empty()) {
        return {ErrorType::UNDECLARED_GLOBAL, "No global scope"};
    }
    auto& currentScope = scopeStack.top().symbols;
    for (const auto& sym : currentScope) {
        if (std::holds_alternative<std::string>(sym.name) &&
            std::holds_alternative<std::string>(symbol.name) &&
            std::get<std::string>(sym.name) == std::get<std::string>(symbol.name)) {
            std::string invalidPidentifier = std::get<std::string>(sym.name);
            return {ErrorType::DUPLICATE_DECLARATION, invalidPidentifier};
        }
        if (sym.type == semana::VariableType::ARRAY) {
            if (std::holds_alternative<ast::array>(sym.name)) {
                auto array = std::get<ast::array>(sym.name);
                if (array.num1 > array.num2) {
                    std::string invalidRange = std::to_string(array.num1) + ":" +
                                               std::to_string(array.num2);
                    return {ErrorType::INCORRECT_ARRAY_RANGE, invalidRange};
                }
            }
        }
    }
    return {ErrorType::GOOD, "Good"};
}

ValidationMessage SymbolTable::validateSymbol(Symbol& symbol) {
    if (scopeStack.empty()) {
        return {ErrorType::UNDECLARED_GLOBAL, "No global scope"};
    }

    auto copyStack = scopeStack;
    auto& currentScope = copyStack.top().symbols;

    if (symbol.symbolType == PROCEDURE_NAME) {
        // get deeper scope
        if (copyStack.size() < 2) {
            return {ErrorType::UNDECLARED_PROCEDURE, "Undeclared procedure"};
        }
        auto topScope = copyStack.top();
        copyStack.pop();
        auto deeperScope = copyStack.top();
        currentScope = deeperScope.symbols;
    }

    std::optional<Symbol> symbolFromTable;
    for (const auto& sym : currentScope) {
        if (std::holds_alternative<std::string>(sym.name) && 
            std::holds_alternative<std::string>(symbol.name) &&
            std::get<std::string>(sym.name) == std::get<std::string>(symbol.name)) {
            symbolFromTable = sym;
            break;
        } else if (std::holds_alternative<ast::array>(sym.name) && 
                   std::holds_alternative<ast::array>(symbol.name) &&   
                   std::get<ast::array>(sym.name) == std::get<ast::array>(symbol.name)) {
            symbolFromTable = sym;
            break;
        }
        else if(std::holds_alternative<std::string>(sym.name) && 
                std::holds_alternative<ast::array>(symbol.name)){
            if(std::get<std::string>(sym.name) == std::get<ast::array>(symbol.name).pidentifier){
                symbolFromTable = sym;
                break;
            }
        }
        // below thereis a situation when we pass an array as a function argument - then symbol is treated as num
        else if(std::holds_alternative<ast::array>(sym.name) && 
                std::holds_alternative<std::string>(symbol.name)){
            if(std::get<ast::array>(sym.name).pidentifier == std::get<std::string>(symbol.name)){
                symbolFromTable = sym;
                symbol.symbolType = ARRAY_NAME;
                break;
            }
        }
    }

    if(symbolFromTable.has_value() && symbolFromTable.value().type == VariableType::FOR_ITERATOR && symbol.type == VariableType::NUM){
        return {ErrorType::ITERATOR, "Validate if is in loop scope"};
    }

    if (!symbolFromTable.has_value()) {
        switch (symbol.symbolType) {
            case VARIABLE_NAME: {
                if (std::holds_alternative<std::string>(symbol.name)) {
                    std::string invalidPidentifier = std::get<std::string>(symbol.name);
                    return {ErrorType::UNDECLARED_VARIABLE, invalidPidentifier};
                }
                break;
            }
            case ARRAY_NAME: {
                if (std::holds_alternative<ast::array>(symbol.name)) {
                    std::string invalidArray = std::get<ast::array>(symbol.name).pidentifier;
                    return {ErrorType::UNDECLARED_ARRAY, invalidArray};
                }
                break;
            }
            case PROCEDURE_NAME: {
                if (std::holds_alternative<std::string>(symbol.name)) {
                    std::string invalidProcedure = std::get<std::string>(symbol.name);
                    return {ErrorType::UNDECLARED_PROCEDURE, invalidProcedure};
                }
                break;
            }
            case MAIN_NAME: {
                return {ErrorType::UNDECLARED_MAIN, "Undeclared main"};
            }
            default: {
                throw std::runtime_error("Unknown symbol type");
            }
        }
    }

    switch (symbol.symbolType) {
        case VARIABLE_NAME: {
            if (symbolFromTable->type == semana::VariableType::ARRAY) {
                if (std::holds_alternative<std::string>(symbol.name)) {
                    std::string invalidPidentifier = std::get<std::string>(symbol.name);
                    return {ErrorType::TYPE_MISMATCH,
                            "Expected array, got variable. " + invalidPidentifier};
                }
            }
            if (symbolFromTable->type != symbol.type) {
                if (std::holds_alternative<std::string>(symbol.name)) {
                    std::string invalidPidentifier = std::get<std::string>(symbol.name);
                    return {ErrorType::TYPE_MISMATCH,
                            "Expected variable, got something else. " +
                                invalidPidentifier};
                }
            }
            break;
        }
        case ARRAY_NAME: {
            if (symbolFromTable->type == semana::VariableType::NUM) {
                if (std::holds_alternative<ast::array>(symbol.name)) {
                    std::string invalidArray = std::get<ast::array>(symbol.name).pidentifier;
                    return {ErrorType::TYPE_MISMATCH,
                            "Expected variable, got array. " + invalidArray};
                }
            }
            if (symbolFromTable->type != symbol.type) {
                if (std::holds_alternative<ast::array>(symbol.name)) {
                    std::string invalidArray = std::get<ast::array>(symbol.name).pidentifier;
                    return {ErrorType::TYPE_MISMATCH,
                            "Expected array, got something else. " + invalidArray};
                }
            }
            break;
        }
        case PROCEDURE_NAME: {
            if (symbolFromTable->type != symbol.type) {
                if (std::holds_alternative<std::string>(symbol.name)) {
                    std::string invalidProcedure = std::get<std::string>(symbol.name);
                    return {ErrorType::TYPE_MISMATCH,
                            "Expected procedure, got something else. " +
                                invalidProcedure};
                }
            }
            break;
        }
        case MAIN_NAME: {
            if (symbolFromTable->type != symbol.type) {
                return {ErrorType::TYPE_MISMATCH,
                        "Expected main, got something else"};
            }
            break;
        }
        default: {
            throw std::runtime_error("Unknown symbol type");
        }
    }

    return {ErrorType::GOOD, "Good"};
}

void SymbolTable::printSymbols() {
    std::stack<Scope> tempStack = scopeStack;
    while (!tempStack.empty()) {
        std::cout << "Scope: " << tempStack.top().name << std::endl;
        auto& currentScope = tempStack.top().symbols;
        for (const auto& sym : currentScope) {
            if (std::holds_alternative<std::string>(sym.name)) {
                std::cout << "Variable: " << std::get<std::string>(sym.name)
                          << std::endl;
            } else {
                auto array = std::get<ast::array>(sym.name);
                std::cout << "Array: " << array.pidentifier << " ["
                          << array.num1 << ":" << array.num2 << "]"
                          << std::endl;
            }
        }
        tempStack.pop();
    }
}

}  // namespace semana
