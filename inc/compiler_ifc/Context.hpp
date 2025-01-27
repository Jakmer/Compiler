#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "ProgramAllNode.hpp"
#include "SymbolTable.hpp"

namespace compiler {

class Context {
public:
    ast::ProgramAllNode* astRoot = nullptr;
    semana::SymbolTable symbolTable;
    std::string outputFile;

    Context() = default;
    ~Context() = default;
};

} // namespace compiler

#endif  // !CONTEXT_HPP
