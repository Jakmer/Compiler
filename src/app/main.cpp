#include <FlexLexer.h>
#include <fstream>
#include <iostream>
#include "tokens.hpp"

static yyFlexLexer lexer;

int yylex() { return lexer.yylex(); }

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

    std::cout << "Starting lexer...\n";
    int token;
    while ((token = yylex()) != 0) {
        std::cout << static_cast<token::TokenType>(token) << " : " << lexer.YYText() << "\n";
    }
    std::cout << "Lexer finished.\n";

    return 0;
}
