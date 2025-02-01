#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <string>

#include "Symbol.hpp"

namespace semana {

class SymbolTable {
   public:
    SymbolTable();
    ValidationMessage openScope(ScopeType scopeType);
    ValidationMessage closeScope();
    ValidationMessage addSymbol(Symbol &symbol, RuntimeParams &runtimeParams);
    ValidationMessage validateSymbol(Symbol &symbol,
                                     semana::RuntimeParams &runtimeParams);
    Symbol getSymbolByName(std::string &name, int &scope);
    int getScopeByProcName(std::string &name);
    unsigned long getProcedureAddr(std::string &name);
    std::vector<RValue> getRValues();
    unsigned long getLastUsedAddr();
    unsigned long getAddrOfProcArg(std::string &procName, int &noArg);
    void addProcArgs(std::string &procName, std::unordered_map<int, std::string> &args);
    bool isProcArgument(std::string &argName, std::string &procName);

   private:
    ValidationMessage validateDeclaration(Symbol &symbol,
                                          RuntimeParams &runtimeParams);
    std::string getSymbolUniqeName(Symbol &symbol, int scope = -2);
    Symbol &getLatestProcedureName(std::string &name);
    void assignAddress(Symbol &symbol);

    std::stack<int> scopes;                           // only for scope checking
    std::unordered_map<std::string, Symbol> symbols;  // name and symbol
    int noProcedures;
    unsigned long address;
    std::unordered_map<std::string, std::unordered_map<int, std::string>> procedureArgs;
};

}  // namespace semana

#endif  // SYMBOL_TABLE_HPP
