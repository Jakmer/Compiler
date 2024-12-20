%{
#include <iostream>
#include <string>

extern int yylex();
extern int yyparse();
int yyerror(const char* s);

%}

%union {
    int num;
    char* id;
}

%token PROGRAM PROCEDURE IS EBEGIN END IF THEN ELSE ENDIF WHILE DO ENDWHILE REPEAT UNTIL FOR FROM TO DOWNTO ENDFOR READ WRITE
%token PLUS MINUS TIMES DIVIDE MOD ASSIGN EQUALS NEQ GREATER LESS GEQ LEQ COLON SEMICOLON COMMA LPAREN RPAREN LBRACKET RBRACKET
%token pidentifier num
%token T

%type <num> num
%type <id> pidentifier

%%

program_all:
    procedures main
    ;

procedures:
    procedures PROCEDURE proc_head IS declarations EBEGIN commands END
    | procedures PROCEDURE proc_head IS EBEGIN commands END
    | /* empty */
    ;

main:
    PROGRAM IS declarations EBEGIN commands END
    | PROGRAM IS EBEGIN commands END
    ;

commands:
    commands command
    | command
    ;

command:
    identifier ASSIGN expression SEMICOLON
    | IF condition THEN commands ELSE commands ENDIF
    | IF condition THEN commands ENDIF
    | WHILE condition DO commands ENDWHILE
    | REPEAT commands UNTIL condition SEMICOLON
    | FOR pidentifier FROM value TO value DO commands ENDFOR
    | FOR pidentifier FROM value DOWNTO value DO commands ENDFOR
    | proc_call SEMICOLON
    | READ identifier SEMICOLON
    | WRITE value SEMICOLON
    ;

proc_head:
    pidentifier LPAREN args_decl RPAREN
    ;

proc_call:
    pidentifier LPAREN args RPAREN
    ;

declarations:
    declarations COMMA pidentifier
    | declarations COMMA pidentifier LBRACKET num COLON num RBRACKET
    | pidentifier
    | pidentifier LBRACKET num COLON num RBRACKET
    ;

args_decl:
    args_decl COMMA pidentifier
    | args_decl COMMA T pidentifier
    | pidentifier
    | T pidentifier
    ;

args:
    args COMMA pidentifier
    | pidentifier
    ;

expression:
    value
    | value PLUS value
    | value MINUS value
    | value TIMES value
    | value DIVIDE value
    | value MOD value
    ;

condition:
    value EQUALS value
    | value NEQ value
    | value GREATER value
    | value LESS value
    | value GEQ value
    | value LEQ value
    ;

value:
    num { std::cout << "Found a number: " << $1 << std::endl; }
    | identifier
    ;

identifier:
    pidentifier
    | pidentifier LBRACKET pidentifier RBRACKET
    | pidentifier LBRACKET num RBRACKET
    ;

%%

int yyerror(const char* s) {
    fprintf(stderr, "Parser error: %s\n", s);
    return 0;
}

