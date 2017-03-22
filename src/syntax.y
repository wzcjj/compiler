%{
#include <stdio.h>
#include <string.h>
%}
%token INT FLOAT ID
%token ASSIGNOP
%token OR
%token AND
%token RELOP
%token PLUS MINUS
%token STAR DIV
%token NOT NEG
%token LP RP LB RB DOT
%token LC RC
%token TYPE STRUCT RETURN IF WHILE ELSE
%token COMMA SEMI
%%
/* High-level Definitions */
Program : ExtDefList {}
        ;
ExtDefList : {}
           | ExtDef ExtDefList {};
ExtDef : Specifier ExtDecList SEMI {}
       | Specifier SEMI {}
       | Specifier FunDec CompSt {};
ExtDecList : VarDec {}
           | VarDec COMMA ExtDecList {};
%%
