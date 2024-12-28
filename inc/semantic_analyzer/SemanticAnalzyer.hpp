#ifndef SEMANTIC_ANALYZER_HPP
#define SEMANTIC_ANALYZER_HPP

#include <map>
#include <string>
#include <vector>

#include "ASTNode.hpp"
#include "SymbolTable.hpp"

namespace semana {

class SemanticAnalyzer {
   public:
    SemanticAnalyzer(std::string &filename);
    ~SemanticAnalyzer();

    void analyze(ASTNode* root);
    bool hasErrors() const;
    const std::vector<std::string>& getErrors() const;

   private:
    void checkVariableDeclarations(const std::vector<std::string>& tokens);
    void checkFunctionDefinitions(const std::vector<std::string>& tokens);
    void raiseError(ValidationMessage &message, std::string &location);
    void processNode(ASTNode* node);
    void printSymbolTable();
    std::string getLocation(ASTNode* node);
    ValidationMessage validateIterator(ASTNode* node);

    std::vector<std::string> errors;
    SymbolTable symbolTable;
    std::string filename;
    ASTNode* astRoot;
};

}  // namespace semana

#endif  // SEMANTIC_ANALYZER_HPP
