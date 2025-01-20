%{
#include <iostream>
#include <string>
#include "AST.hpp"

ast::ProgramAllNode* astRoot = nullptr;

extern int yylex();
extern int yyparse();
int yyerror(const char* s);

#define YYDEBUG 1

%}

%debug
%locations

%union {
    int int_value;
    char* string_value;
    class ASTNode* node_value;
}

%token PROGRAM PROCEDURE IS EBEGIN END IF THEN ELSE ENDIF WHILE DO ENDWHILE REPEAT UNTIL FOR FROM TO DOWNTO ENDFOR READ WRITE
%token PLUS MINUS TIMES DIVIDE MOD ASSIGN EQUALS NEQ GREATER LESS GEQ LEQ COLON SEMICOLON COMMA LPAREN RPAREN LBRACKET RBRACKET
%token pidentifier num
%token T

%type <node_value> program_all main command procedures expression condition value identifier proc_head proc_call declarations args_decl args commands
%type <string_value> num
%type <string_value> pidentifier

%%

program_all:
    procedures main {
        ast::ProgramAllNode* programAllNode = dynamic_cast<ast::ProgramAllNode*>($1);
        programAllNode->main = dynamic_cast<ast::MainNode*>($2);
        astRoot = programAllNode;
        $$ = programAllNode;
    }
    ;

procedures:
    procedures PROCEDURE proc_head IS declarations EBEGIN commands END {
        ast::ProceduresNode* procedureNode = new ast::ProceduresNode();
        ast::ProcHeadNode* procHeadNode = dynamic_cast<ast::ProcHeadNode*>($3);
        ast::DeclarationsNode* declarationsNode = dynamic_cast<ast::DeclarationsNode*>($5);
        ast::CommandsNode* commandsNode = dynamic_cast<ast::CommandsNode*>($7);
        procedureNode->proc_head = procHeadNode;
        procedureNode->declarations = declarationsNode;
        procedureNode->commands = commandsNode;

        ast::ProgramAllNode* programAllNode = dynamic_cast<ast::ProgramAllNode*>($1);
        programAllNode->procedures.push_back(procedureNode);

        $$ = programAllNode;
    }
    | procedures PROCEDURE proc_head IS EBEGIN commands END {
        ast::ProceduresNode* procedureNode = new ast::ProceduresNode();
        ast::ProcHeadNode* procHeadNode = dynamic_cast<ast::ProcHeadNode*>($3);
        ast::CommandsNode* commandsNode = dynamic_cast<ast::CommandsNode*>($6);
        procedureNode->proc_head = procHeadNode;
        procedureNode->commands = commandsNode;

        ast::ProgramAllNode* programAllNode = dynamic_cast<ast::ProgramAllNode*>($1);
        programAllNode->procedures.push_back(procedureNode);

        $$ = programAllNode;
    }
    | /* empty */ {
        ast::ProgramAllNode* programAllNode = new ast::ProgramAllNode();
        $$ = programAllNode;
    }
    ;

main:
    PROGRAM IS declarations EBEGIN commands END {
        ast::MainNode* mainNode = new ast::MainNode();
        mainNode->declarations = dynamic_cast<ast::DeclarationsNode*>($3);
        mainNode->commands = dynamic_cast<ast::CommandsNode*>($5);
        $$ = mainNode;
    }
    | PROGRAM IS EBEGIN commands END {
        ast::MainNode* mainNode = new ast::MainNode();
        mainNode->commands = dynamic_cast<ast::CommandsNode*>($4);
        $$ = mainNode;
    }
    ;

commands:
    commands command {
        ast::CommandsNode* commandsNode = dynamic_cast<ast::CommandsNode*>($1);
        commandsNode->commands.push_back($2);       // TODO: Fix: Losing type of command
        $$ = commandsNode;
    }
    | command {
        ast::CommandsNode* commandsNode = new ast::CommandsNode();
        commandsNode->commands.push_back($1);
        $$ = commandsNode;
    }
    ;

command:
    identifier ASSIGN expression SEMICOLON {
        ast::IdentifierNode* identifierNode = dynamic_cast<ast::IdentifierNode*>($1);
        ast::ExpressionNode* expressionNode = dynamic_cast<ast::ExpressionNode*>($3);
        ast::AssignmentNode* commandNode = new ast::AssignmentNode();
        commandNode->identifier = identifierNode;
        commandNode->expression = expressionNode;

        $$ = commandNode;
    }
    | IF condition THEN commands ELSE commands ENDIF {
        ast::ConditionNode* conditionNode = dynamic_cast<ast::ConditionNode*>($2);
        ast::CommandsNode* commandsNodeIf = dynamic_cast<ast::CommandsNode*>($4);
        ast::CommandsNode* commandsNodeIfElse = dynamic_cast<ast::CommandsNode*>($6);

        ast::IfStatementNode* ifStatementNode = new ast::IfStatementNode();
        ifStatementNode->condition = conditionNode;
        ifStatementNode->commands = commandsNodeIf;
        ifStatementNode->elseCommands = commandsNodeIfElse;

        $$ = ifStatementNode;
    }
    | IF condition THEN commands ENDIF {
        ast::ConditionNode* conditionNode = dynamic_cast<ast::ConditionNode*>($2);
        ast::CommandsNode* commandsNodeIf = dynamic_cast<ast::CommandsNode*>($4);

        ast::IfStatementNode* ifStatementNode = new ast::IfStatementNode();
        ifStatementNode->condition = conditionNode;
        ifStatementNode->commands = commandsNodeIf;

        $$ = ifStatementNode;
    }
    | WHILE condition DO commands ENDWHILE {
        ast::ConditionNode* conditionNode = dynamic_cast<ast::ConditionNode*>($2);
        ast::CommandsNode* commandsNode = dynamic_cast<ast::CommandsNode*>($4);

        ast::WhileStatementNode* whileStatementNode = new ast::WhileStatementNode();
        whileStatementNode->condition = conditionNode;
        whileStatementNode->commands = commandsNode;

        $$ = whileStatementNode;
    }
    | REPEAT commands UNTIL condition SEMICOLON {
        ast::ConditionNode* conditionNode = dynamic_cast<ast::ConditionNode*>($4);
        ast::CommandsNode* commandsNode = dynamic_cast<ast::CommandsNode*>($2);

        ast::RepeatStatementNode* repeatStatementNode = new ast::RepeatStatementNode();
        repeatStatementNode->condition = conditionNode;
        repeatStatementNode->commands = commandsNode;

        $$ = repeatStatementNode;
    }
    | FOR pidentifier FROM value TO value DO commands ENDFOR {
        std::string identifier = $2;
        ast::ValueNode* valueNode1 = dynamic_cast<ast::ValueNode*>($4);
        ast::ValueNode* valueNode2 = dynamic_cast<ast::ValueNode*>($6);
        ast::CommandsNode* commandsNode = dynamic_cast<ast::CommandsNode*>($8);

        ast::ForToNode* forStatementNode = new ast::ForToNode();
        forStatementNode->pidentifier = identifier;
        forStatementNode->valueFrom = valueNode1;
        forStatementNode->valueTo = valueNode2;
        forStatementNode->commands = commandsNode;

        $$ = forStatementNode;
    }
    | FOR pidentifier FROM value DOWNTO value DO commands ENDFOR {
        std::string identifier = $2;
        ast::ValueNode* valueNode1 = dynamic_cast<ast::ValueNode*>($4);
        ast::ValueNode* valueNode2 = dynamic_cast<ast::ValueNode*>($6);
        ast::CommandsNode* commandsNode = dynamic_cast<ast::CommandsNode*>($8);

        ast::ForDowntoNode* forStatementNode = new ast::ForDowntoNode();
        forStatementNode->pidentifier = identifier;
        forStatementNode->valueFrom = valueNode1;
        forStatementNode->valueTo = valueNode2;
        forStatementNode->commands = commandsNode;

        $$ = forStatementNode;
    }
    | proc_call SEMICOLON {
        ast::ProcCallNode* procCallNode = dynamic_cast<ast::ProcCallNode*>($1);
        $$ = procCallNode;
    }
    | READ identifier SEMICOLON {
        ast::IdentifierNode* identifierNode = dynamic_cast<ast::IdentifierNode*>($2);
        ast::ReadNode* readNode = new ast::ReadNode();
        readNode->identifier = identifierNode;
        $$ = readNode;
    }
    | WRITE value SEMICOLON {
        ast::ValueNode* valueNode = dynamic_cast<ast::ValueNode*>($2);
        ast::WriteNode* writeNode = new ast::WriteNode();
        writeNode->value = valueNode;
        $$ = writeNode;
    }
    ;

proc_head:
    pidentifier LPAREN args_decl RPAREN {
        ast::ProcHeadNode* procHeadNode = new ast::ProcHeadNode();
        procHeadNode->pidentifier = $1;
        procHeadNode->setPosition(@1.first_line, @1.first_column);
        procHeadNode->args_decl = dynamic_cast<ast::ArgsDeclNode*>($3);
        $$ = procHeadNode;
    }
    ;

proc_call:
    pidentifier LPAREN args RPAREN {
        ast::ProcCallNode* procCallNode = new ast::ProcCallNode();
        procCallNode->pidentifier = $1;
        procCallNode->setPosition(@1.first_line, @1.first_column);
        procCallNode->args = dynamic_cast<ast::ArgsNode*>($3);
        $$ = procCallNode;
    }
    ;

declarations:
    declarations COMMA pidentifier {
        ast::DeclarationsNode* declarationsNode = dynamic_cast<ast::DeclarationsNode*>($1);
        declarationsNode->pidentifiers.push_back($3);   // TODO: Add more positions for each identifier
        declarationsNode->setPosition(@3.first_line, @3.first_column);
        $$ = declarationsNode;
    }
    | declarations COMMA pidentifier LBRACKET num COLON num RBRACKET {
        ast::DeclarationsNode* declarationsNode = dynamic_cast<ast::DeclarationsNode*>($1);
        ast::array arr{ $3, $5, $7 };
        declarationsNode->arrays.push_back(arr);
        declarationsNode->setPosition(@3.first_line, @3.first_column);
        $$ = declarationsNode;
    }
    | pidentifier {
        ast::DeclarationsNode* declarationsNode = new ast::DeclarationsNode();
        declarationsNode->pidentifiers.push_back($1);
        declarationsNode->setPosition(@1.first_line, @1.first_column);
        $$ = declarationsNode;
    }
    | pidentifier LBRACKET num COLON num RBRACKET {
        ast::DeclarationsNode* declarationsNode = new ast::DeclarationsNode();
        ast::array arr { $1, $3, $5 };
        declarationsNode->arrays.push_back(arr);
        declarationsNode->setPosition(@1.first_line, @1.first_column);
        $$ = declarationsNode;
    }
    ;

args_decl:
    args_decl COMMA pidentifier {
        ast::ArgsDeclNode* argsDeclNode = dynamic_cast<ast::ArgsDeclNode*>($1);
        argsDeclNode->pidentifiers.push_back($3);
        argsDeclNode->setPosition(@3.first_line, @3.first_column);
        $$ = argsDeclNode;
    }
    | args_decl COMMA T pidentifier {
        ast::ArgsDeclNode* argsDeclNode = dynamic_cast<ast::ArgsDeclNode*>($1);
        argsDeclNode->Tpidentifiers.push_back($4);
        argsDeclNode->setPosition(@4.first_line, @4.first_column);
        $$ = argsDeclNode;
    }
    | pidentifier {
        ast::ArgsDeclNode* argsDeclNode = new ast::ArgsDeclNode();
        argsDeclNode->pidentifiers.push_back($1);
        argsDeclNode->setPosition(@1.first_line, @1.first_column);
        $$ = argsDeclNode;
    }
    | T pidentifier {
        ast::ArgsDeclNode* argsDeclNode = new ast::ArgsDeclNode();
        argsDeclNode->Tpidentifiers.push_back($2);
        argsDeclNode->setPosition(@2.first_line, @2.first_column);
        $$ = argsDeclNode;
    }
    ;

args:
    args COMMA pidentifier {
        ast::ArgsNode* argsNode = dynamic_cast<ast::ArgsNode*>($1);
        argsNode->pidentifiers.push_back($3);
        argsNode->setPosition(@3.first_line, @3.first_column);
        $$ = argsNode;
    }
    | pidentifier {
        ast::ArgsNode* argsNode = new ast::ArgsNode();
        argsNode->pidentifiers.push_back($1);
        argsNode->setPosition(@1.first_line, @1.first_column);
        $$ = argsNode;
    }
    ;

expression:
    value {
        ASTNode* node1 = $1;
        node1->setPosition(@1.first_line, @1.first_column);
        ast::ValueNode* valueNode1 = dynamic_cast<ast::ValueNode*>(node1);

        ast::ExpressionNode* expressionNode = new ast::ExpressionNode();
        expressionNode->value1 = valueNode1;

        $$ = expressionNode;
    }
    | value PLUS value {
        ASTNode* node1 = $1;
        ASTNode* node2 = $3;
        ast::ValueNode* valueNode1 = dynamic_cast<ast::ValueNode*>(node1);
        ast::ValueNode* valueNode2 = dynamic_cast<ast::ValueNode*>(node2);

        ast::ExpressionNode* expressionNode = new ast::ExpressionNode();
        expressionNode->value1 = valueNode1;
        expressionNode->value2 = valueNode2;
        expressionNode->mathOperation = ast::MathOperation::PLUS;

        $$ = expressionNode;
    }
    | value MINUS value {
        ASTNode* node1 = $1;
        ASTNode* node2 = $3;
        ast::ValueNode* valueNode1 = dynamic_cast<ast::ValueNode*>(node1);
        ast::ValueNode* valueNode2 = dynamic_cast<ast::ValueNode*>(node2);

        ast::ExpressionNode* expressionNode = new ast::ExpressionNode();
        expressionNode->value1 = valueNode1;
        expressionNode->value2 = valueNode2;
        expressionNode->mathOperation = ast::MathOperation::SUBSTRACT;

        $$ = expressionNode;
    }
    | value TIMES value {
        ASTNode* node1 = $1;
        ASTNode* node2 = $3;
        ast::ValueNode* valueNode1 = dynamic_cast<ast::ValueNode*>(node1);
        ast::ValueNode* valueNode2 = dynamic_cast<ast::ValueNode*>(node2);

        ast::ExpressionNode* expressionNode = new ast::ExpressionNode();
        expressionNode->value1 = valueNode1;
        expressionNode->value2 = valueNode2;
        expressionNode->mathOperation = ast::MathOperation::MULTIPLY;

        $$ = expressionNode;
    }
    | value DIVIDE value {
        ASTNode* node1 = $1;
        ASTNode* node2 = $3;
        ast::ValueNode* valueNode1 = dynamic_cast<ast::ValueNode*>(node1);
        ast::ValueNode* valueNode2 = dynamic_cast<ast::ValueNode*>(node2);

        ast::ExpressionNode* expressionNode = new ast::ExpressionNode();
        expressionNode->value1 = valueNode1;
        expressionNode->value2 = valueNode2;
        expressionNode->mathOperation = ast::MathOperation::DIVIDE;

        $$ = expressionNode;
    }
    | value MOD value {
        ASTNode* node1 = $1;
        ASTNode* node2 = $3;
        ast::ValueNode* valueNode1 = dynamic_cast<ast::ValueNode*>(node1);
        ast::ValueNode* valueNode2 = dynamic_cast<ast::ValueNode*>(node2);

        ast::ExpressionNode* expressionNode = new ast::ExpressionNode();
        expressionNode->value1 = valueNode1;
        expressionNode->value2 = valueNode2;
        expressionNode->mathOperation = ast::MathOperation::MOD;

        $$ = expressionNode;
    }
    ;

condition:
    value EQUALS value {
        ast::ValueNode* valueNode1 = dynamic_cast<ast::ValueNode*>($1);
        ast::ValueNode* valueNode2 = dynamic_cast<ast::ValueNode*>($3);

        ast::ConditionNode* conditionNode = new ast::ConditionNode();
        conditionNode->value1 = valueNode1;
        conditionNode->value2 = valueNode2;
        conditionNode->relation = ast::Relation::EQ;

        $$ = conditionNode;
    }
    | value NEQ value {
        ast::ValueNode* valueNode1 = dynamic_cast<ast::ValueNode*>($1);
        ast::ValueNode* valueNode2 = dynamic_cast<ast::ValueNode*>($3);

        ast::ConditionNode* conditionNode = new ast::ConditionNode();
        conditionNode->value1 = valueNode1;
        conditionNode->value2 = valueNode2;
        conditionNode->relation = ast::Relation::NEQ;

        $$ = conditionNode;
    }
    | value GREATER value {
        ast::ValueNode* valueNode1 = dynamic_cast<ast::ValueNode*>($1);
        ast::ValueNode* valueNode2 = dynamic_cast<ast::ValueNode*>($3);

        ast::ConditionNode* conditionNode = new ast::ConditionNode();
        conditionNode->value1 = valueNode1;
        conditionNode->value2 = valueNode2;
        conditionNode->relation = ast::Relation::GT;

        $$ = conditionNode;
    }
    | value LESS value {
        ast::ValueNode* valueNode1 = dynamic_cast<ast::ValueNode*>($1);
        ast::ValueNode* valueNode2 = dynamic_cast<ast::ValueNode*>($3);

        ast::ConditionNode* conditionNode = new ast::ConditionNode();
        conditionNode->value1 = valueNode1;
        conditionNode->value2 = valueNode2;
        conditionNode->relation = ast::Relation::LT;

        $$ = conditionNode;
    }
    | value GEQ value {
        ast::ValueNode* valueNode1 = dynamic_cast<ast::ValueNode*>($1);
        ast::ValueNode* valueNode2 = dynamic_cast<ast::ValueNode*>($3);

        ast::ConditionNode* conditionNode = new ast::ConditionNode();
        conditionNode->value1 = valueNode1;
        conditionNode->value2 = valueNode2;
        conditionNode->relation = ast::Relation::GE;

        $$ = conditionNode;
    }
    | value LEQ value {
        ast::ValueNode* valueNode1 = dynamic_cast<ast::ValueNode*>($1);
        ast::ValueNode* valueNode2 = dynamic_cast<ast::ValueNode*>($3);

        ast::ConditionNode* conditionNode = new ast::ConditionNode();
        conditionNode->value1 = valueNode1;
        conditionNode->value2 = valueNode2;
        conditionNode->relation = ast::Relation::LE;

        $$ = conditionNode;
    }
    ;

value:
    num { ast::ValueNode* node = new ast::ValueNode();
          node->num = $1;
          $$ = node;
          }
    | identifier {
        ast::ValueNode* node = new ast::ValueNode();
        node->identifier = dynamic_cast<ast::IdentifierNode*>($1);
        $$ = node;
    }
    ;

identifier:
    pidentifier {
        ast::IdentifierNode* node = new ast::IdentifierNode();
        node->pidentifier = $1;
        node->setPosition(@1.first_line, @1.first_column);
        $$ = node;
    }
    | pidentifier LBRACKET pidentifier RBRACKET {
        ast::IdentifierNode* node = new ast::IdentifierNode();
        node->Tpidentifier = $1;
        node->arrayPidentifierIndex = $3;
        node->setPosition(@1.first_line, @1.first_column);
        $$ = node;
    }
    | pidentifier LBRACKET num RBRACKET {
        ast::IdentifierNode* node = new ast::IdentifierNode();
        node->Tpidentifier = $1;
        node->arrayNumIndex = $3;
        node->setPosition(@1.first_line, @1.first_column);
        $$ = node;
    }
    ;

%%

int yyerror(const char* s) {
    fprintf(stderr, "Parse error at line %d, column %d: %s\n", 
            yylloc.first_line, yylloc.first_column, s);
    return 0;
}

