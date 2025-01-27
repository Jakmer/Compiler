#ifndef SEMANTIC_ANALYZER_HPP
#define SEMANTIC_ANALYZER_HPP

#include <string>
#include <vector>

#include "ASTNode.hpp"
#include "ErrorMessages.hpp"
#include "SymbolTable.hpp"
#include "Context.hpp"

namespace semana {

class SemanticAnalyzer {
   public:
    SemanticAnalyzer(std::string &filename);
    ~SemanticAnalyzer();

    ExitCode analyze(compiler::Context &context);
    bool hasErrors() const;
    const std::vector<std::string>& getErrors() const;

   private:
    void checkVariableDeclarations(const std::vector<std::string>& tokens);
    void checkFunctionDefinitions(const std::vector<std::string>& tokens);
    void raiseError(ValidationMessage &message, std::string &location);
    void processNode(ASTNode* node);
    std::string getLocation(ASTNode* node);
    ValidationMessage validateIterator(ASTNode* node);

    std::vector<std::string> errors;
    SymbolTable symbolTable;
    std::string filename;
    ASTNode* astRoot;
    RuntimeParams runtimeParams;
    ExitCode exitCode;
};

}  // namespace semana

#endif  // SEMANTIC_ANALYZER_HPP
