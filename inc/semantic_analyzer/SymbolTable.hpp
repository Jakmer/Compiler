#include <iostream>
#include <stack>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "DeclarationsNode.hpp"

namespace semana {

enum class VariableType {
    NUM,
    ARRAY,
    PROCEDURE,
    MAIN,
    UNDEFINED,
    FOR_ITERATOR
};

enum ScopeType { GLOBAL, PROCEDURE, MAIN };

enum SymbolType {
    MAIN_NAME,
    PROCEDURE_NAME,
    VARIABLE_NAME,
    ARRAY_NAME,
    UNDEFINED
};

struct Symbol {
    SymbolType symbolType;
    VariableType type;
    std::variant<std::string, ast::array> name;

    Symbol()
        : symbolType(SymbolType::UNDEFINED),
          type(VariableType::UNDEFINED),
          name("undefined") {}
    Symbol(SymbolType symbolType, VariableType type = VariableType::UNDEFINED,
           std::variant<std::string, ast::array> name = "undefined")
        : symbolType(symbolType), type(type), name(name) {}
};

struct Scope {
    ScopeType scopeType;
    std::string name;
    std::vector<Symbol> symbols;
};

enum ErrorType {
    GOOD,
    UNDECLARED_VARIABLE,
    INCORRECT_ARRAY_RANGE,
    DUPLICATE_DECLARATION,
    UNDECLARED_PROCEDURE,
    INCORRECT_ARGUMENTS,
    UNDECLARED_ARRAY,
    UNDECLARED_MAIN,
    UNDECLARED_GLOBAL,
    TYPE_MISMATCH,
    ITERATOR // when receive ITERATOR, we should check on AST whether is in RANGE
};

struct ValidationMessage {
    ErrorType errorType;
    std::string content;
};

class SymbolTable {
   public:
    SymbolTable() = default;
    void insert(const Symbol& symbol);
    void printSymbols();
    void openScope(std::string& name, ScopeType scopeType);
    void closeScope();
    ValidationMessage validateSymbol(Symbol& symbol);
    ValidationMessage validateDeclaration(const Symbol& symbol);

   private:
    std::stack<Scope> scopeStack;
};

}  // namespace semana
