%{
    #include <iostream>
    #include <cstdio>
%}

%option c++
%option noyywrap

%%

[a-zA-Z_][a-zA-Z0-9_]*   { std::cout << "Found identifier: " << YYText() << "\n"; }
[0-9]+                  { std::cout << "Found number: " << YYText() << "\n"; }
.                       {}

%%
    extern int yylex();
