#include <FlexLexer.h>

#include <fstream>
#include <iostream>

#include "Parser.hpp"
#include "ProgramAllNode.hpp"

static yyFlexLexer lexer;

int yylex() { return lexer.yylex(); }

extern ast::ProgramAllNode* astRoot;

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Could not open file " << argv[1] << "\n";
        return 1;
    }

    lexer.switch_streams(&file, &std::cout);

    std::cout << "Starting parser ...\n";
    int parseResult = yyparse();
    std::cout << "Parser finished with exit code " << parseResult << ".\n";
    std::cout << "AST:" << '\n';
    astRoot->print();

    return 0;
}
