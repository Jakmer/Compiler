#ifndef TOKENS_HPP
#define TOKENS_HPP

#include <iostream>
#include <string>

namespace token {

enum TokenType {
    // Keywords
    PROGRAM = 256,      // start with 256 to avoid ASCII enumerations conflict
    PROCEDURE,
    IS,
    EBEGIN,
    END,
    IF,
    THEN,
    ELSE,
    ENDIF,
    WHILE,
    DO,
    ENDWHILE,
    REPEAT,
    UNTIL,
    FOR,
    FROM,
    TO,
    DOWNTO,
    ENDFOR,
    READ,
    WRITE,

    // Identifiers and numbers 
    pidentifier,
    num,

    // Operators and symbols
    PLUS,    // "+"
    MINUS,   // "-"
    TIMES,   // "*"
    DIVIDE,  // "/"
    MOD,     // "%"
    EQUALS,  // "="
    NEQ,     // "!="
    GREATER, // ">"
    LESS,    // "<"
    GEQ,     // ">="
    LEQ,     // "<="
    COLON,   // ":"
    SEMICOLON, // ";"
    COMMA,   // ","
    LPAREN,  // "("
    RPAREN,  // ")"
    LBRACKET, // "["
    RBRACKET  // "]"
};

// Function to convert TokenType to string
inline const char* to_string(TokenType type) {
    switch (type) {
        case PROGRAM: return "PROGRAM";
        case PROCEDURE: return "PROCEDURE";
        case IS: return "IS";
        case EBEGIN: return "EBEGIN";
        case END: return "END";
        case IF: return "IF";
        case THEN: return "THEN";
        case ELSE: return "ELSE";
        case ENDIF: return "ENDIF";
        case WHILE: return "WHILE";
        case DO: return "DO";
        case ENDWHILE: return "ENDWHILE";
        case REPEAT: return "REPEAT";
        case UNTIL: return "UNTIL";
        case FOR: return "FOR";
        case FROM: return "FROM";
        case TO: return "TO";
        case DOWNTO: return "DOWNTO";
        case ENDFOR: return "ENDFOR";
        case READ: return "READ";
        case WRITE: return "WRITE";
        case pidentifier: return "pidentifier";
        case num: return "num";
        case PLUS: return "PLUS";
        case MINUS: return "MINUS";
        case TIMES: return "TIMES";
        case DIVIDE: return "DIVIDE";
        case MOD: return "MOD";
        case EQUALS: return "EQUALS";
        case NEQ: return "NEQ";
        case GREATER: return "GREATER";
        case LESS: return "LESS";
        case GEQ: return "GEQ";
        case LEQ: return "LEQ";
        case COLON: return "COLON";
        case SEMICOLON: return "SEMICOLON";
        case COMMA: return "COMMA";
        case LPAREN: return "LPAREN";
        case RPAREN: return "RPAREN";
        case LBRACKET: return "LBRACKET";
        case RBRACKET: return "RBRACKET";
        default: return "UNKNOWN";
    }
}

// Overloading the << operator for TokenType
inline std::ostream& operator<<(std::ostream& os, TokenType type) {
    os << to_string(type);
    return os;
}

} // namespace token

#endif
