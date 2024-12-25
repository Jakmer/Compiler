%{
    #include <iostream>
    #include <cstdio>
    #include "Parser.hpp"

    extern YYSTYPE yylval;
%}

%option c++
%option noyywrap

%%

#[^\n]* { /* Ignore comments */ }
[ \t\n]+               { /* Ignore white spaces */ }
"PROGRAM"           { return PROGRAM; }
"PROCEDURE"         { return PROCEDURE; }
"IS"                { return IS; }
"BEGIN"             { return EBEGIN; }
"END"               { return END; }
"IF"                { return IF; }
"THEN"              { return THEN; }
"ELSE"              { return ELSE; }
"ENDIF"             { return ENDIF; }
"WHILE"             { return WHILE; }
"DO"                { return DO; }
"ENDWHILE"          { return ENDWHILE; }
"REPEAT"            { return REPEAT; }
"UNTIL"             { return UNTIL; }
"FOR"               { return FOR; }
"FROM"              { return FROM; }
"TO"                { return TO; }
"DOWNTO"            { return DOWNTO; }
"ENDFOR"            { return ENDFOR; }
"READ"              { return READ; }
"WRITE"             { return WRITE; }
"T"                 { return T; }


"+"                    { return PLUS; }
"-"                    { return MINUS; }
"*"                    { return TIMES; }
"/"                    { return DIVIDE; }
"%"                    { return MOD; }
":="                   { return ASSIGN; }
"="                    { return EQUALS; }
"!="                   { return NEQ; }
">"                    { return GREATER; }
"<"                    { return LESS; }
">="                   { return GEQ; }
"<="                   { return LEQ; }
":"                    { return COLON; }
";"                    { return SEMICOLON; }
","                    { return COMMA; }
"("                    { return LPAREN; }
")"                    { return RPAREN; }
"["                    { return LBRACKET; }
"]"                    { return RBRACKET; }


[_a-z]+                { yylval.string_value = strdup(yytext); return pidentifier; } // Identifiers
[0-9]+                 { yylval.int_value = atoi(yytext); return num; } // Numbers

.                      { printf("Invalid character: %s\n", yytext); exit(1); }

%%

extern int yylex();
