#include <iostream>
#include <FlexLexer.h>

static yyFlexLexer lexer;

int yylex() {
    return lexer.yylex();
}

int main() {
    std::cout << "Starting lexer...\n";
    yylex();
    std::cout << "Lexer finished.\n";
    return 0;
}
