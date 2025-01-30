%{
    #include <iostream>
    #include <cstdio>
    #include "Parser.hpp"

    extern YYSTYPE yylval;
    extern YYLTYPE yylloc;

    int line_number = 1;
    int column_number = 1;

    #define YY_USER_ACTION \
        yylloc.first_line = line_number; \
        yylloc.last_line = line_number; \
        yylloc.first_column = column_number; \
        yylloc.last_column = column_number + yyleng - 1;

    #define RETURN_TOKEN(token) { column_number += yyleng; return token; }

%}

%option c++
%option noyywrap

%%

#[^\n]* { /* Ignore comments */ }
[ \t]+               { column_number += yyleng; } // Ignore white spaces but update column number
\n                   { line_number++; column_number = 1; } // Update line number and reset column number
"PROGRAM"           { RETURN_TOKEN(PROGRAM); }
"PROCEDURE"         { RETURN_TOKEN(PROCEDURE); }
"IS"                { RETURN_TOKEN(IS); }
"BEGIN"             { RETURN_TOKEN(EBEGIN); }
"END"               { RETURN_TOKEN(END); }
"IF"                { RETURN_TOKEN(IF); }
"THEN"              { RETURN_TOKEN(THEN); }
"ELSE"              { RETURN_TOKEN(ELSE); }
"ENDIF"             { RETURN_TOKEN(ENDIF); }
"WHILE"             { RETURN_TOKEN(WHILE); }
"DO"                { RETURN_TOKEN(DO); }
"ENDWHILE"          { RETURN_TOKEN(ENDWHILE); }
"REPEAT"            { RETURN_TOKEN(REPEAT); }
"UNTIL"             { RETURN_TOKEN(UNTIL); }
"FOR"               { RETURN_TOKEN(FOR); }
"FROM"              { RETURN_TOKEN(FROM); }
"TO"                { RETURN_TOKEN(TO); }
"DOWNTO"            { RETURN_TOKEN(DOWNTO); }
"ENDFOR"            { RETURN_TOKEN(ENDFOR); }
"READ"              { RETURN_TOKEN(READ); }
"WRITE"             { RETURN_TOKEN(WRITE); }
"T"                 { RETURN_TOKEN(T); }

"+"                    { RETURN_TOKEN(PLUS); }
"-"                    { RETURN_TOKEN(MINUS); }
"*"                    { RETURN_TOKEN(TIMES); }
"/"                    { RETURN_TOKEN(DIVIDE); }
"%"                    { RETURN_TOKEN(MOD); }
":="                   { RETURN_TOKEN(ASSIGN); }
"="                    { RETURN_TOKEN(EQUALS); }
"!="                   { RETURN_TOKEN(NEQ); }
">"                    { RETURN_TOKEN(GREATER); }
"<"                    { RETURN_TOKEN(LESS); }
">="                   { RETURN_TOKEN(GEQ); }
"<="                   { RETURN_TOKEN(LEQ); }
":"                    { RETURN_TOKEN(COLON); }
";"                    { RETURN_TOKEN(SEMICOLON); }
","                    { RETURN_TOKEN(COMMA); }
"("                    { RETURN_TOKEN(LPAREN); }
")"                    { RETURN_TOKEN(RPAREN); }
"["                    { RETURN_TOKEN(LBRACKET); }
"]"                    { RETURN_TOKEN(RBRACKET); }

[_a-z]+                { yylval.string_value = strdup(yytext); RETURN_TOKEN(pidentifier); } // Identifiers
-?[0-9]+                 { yylval.string_value = strdup(yytext); RETURN_TOKEN(num); } // Numbers

.                      { printf("Invalid character: %s\n", yytext); exit(1); }

%%

extern int yylex();

