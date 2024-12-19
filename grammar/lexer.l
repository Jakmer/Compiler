%{
    #include <iostream>
    #include <cstdio>
    #include "tokens.hpp"

    union YYSTYPE {
        int num;
        char* id;
    };

    YYSTYPE yylval;
%}

%option c++
%option noyywrap

%%

#[^\n]*                { /* Ignore comments starting with # */ }
"PROGRAM"           { return token::PROGRAM; }
"PROCEDURE"         { return token::PROCEDURE; }
"IS"                { return token::IS; }
"BEGIN"             { return token::EBEGIN; }
"END"               { return token::END; }
"IF"                { return token::IF; }
"THEN"              { return token::THEN; }
"ELSE"              { return token::ELSE; }
"ENDIF"             { return token::ENDIF; }
"WHILE"             { return token::WHILE; }
"DO"                { return token::DO; }
"ENDWHILE"          { return token::ENDWHILE; }
"REPEAT"            { return token::REPEAT; }
"UNTIL"             { return token::UNTIL; }
"FOR"               { return token::FOR; }
"FROM"              { return token::FROM; }
"TO"                { return token::TO; }
"DOWNTO"            { return token::DOWNTO; }
"ENDFOR"            { return token::ENDFOR; }
"READ"              { return token::READ; }
"WRITE"             { return token::WRITE; }

[a-zA-Z_][a-zA-Z0-9_]* { return token::pidentifier; } // Identifiers
[0-9]+                 { yylval.num = atoi(yytext); return token::num; } // Numbers

"+"                   { return token::PLUS; }
"-"                   { return token::MINUS; }
"*"                   { return token::TIMES; }
"/"                   { return token::DIVIDE; }
"%"                   { return token::MOD; }
"="                   { return token::EQUALS; }
"!="                  { return token::NEQ; }
">"                   { return token::GREATER; }
"<"                   { return token::LESS; }
">="                  { return token::GEQ; }
"<="                  { return token::LEQ; }
":"                   { return token::COLON; }
";"                   { return token::SEMICOLON; }
","                   { return token::COMMA; }
"("                   { return token::LPAREN; }
")"                   { return token::RPAREN; }
"["                   { return token::LBRACKET; }
"]"                   { return token::RBRACKET; }

[ \t\n]+              { /* Ignore white spaces */ }
.                     { printf("Invalid character: %s\n", yytext); exit(1); }

%%

extern int yylex();
