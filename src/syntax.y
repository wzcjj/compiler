%{
#include <stdio.h>
#include <string.h>
#include "syntax_tree.h"
#define YYTOKEN_TABLE 1
#define YYSTYPE TreeNode*
#define handle1(type) \
do {\
    int tokennum = yyr2[yyn]; \
    printf("%s ()\n", yytname[YYNTOKENS + type]); \
    yyval = newNode(); \
    int i; \
    for (i = 1; i <= tokennum; i++) { \
        TreeNode *p = yyvsp[i - tokennum]; \
        if (i == 1) (*yyval).lineno = (*p).lineno; \
        treeAddChild(yyval, p); \
    } \
} while (0)
void yyerror(char*);
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
Program : ExtDefList { handle1(1); }
        ;
ExtDefList : { handle1(2); }
           | ExtDef ExtDefList { handle1(2); }
           ;
ExtDef : Specifier ExtDecList SEMI {}
       | Specifier SEMI {}
       | Specifier FunDec CompSt {}
       ;
ExtDecList : VarDec {}
           | VarDec COMMA ExtDecList {}
           ;
/* Specifiers */
Specifier : TYPE {}
          | StructSpecifier {}
          ;
StructSpecifier : STRUCT OptTag LC DefList RC {}
                | STRUCT Tag {}
                ;
OptTag : {}
       | ID {}
       ;
Tag : ID {}
    ;
/* Declarators */
VarDec : ID {}
       | VarDec LB INT RB {}
       ;
FunDec : ID LP VarList RP {}
       | ID LP RP {}
       ;
VarList : ParamDec COMMA VarList {}
        | ParamDec {}
        ;
ParamDec : Specifier VarDec {}
         ;
/* Statements */
CompSt : LC DefList StmtList RC {}
       ;
StmtList : {}
         | Stmt StmtList {}
         ;
Stmt : Exp SEMI {}
     | CompSt {}
     | RETURN Exp SEMI {}
     | IF LP Exp RP Stmt {}
     | IF LP Exp RP Stmt ELSE Stmt {}
     | WHILE LP Exp RP Stmt {}
     ;
/* Local Definitions */
DefList : {}
        | Def DefList {}
        ;
Def : Specifier DecList SEMI {}
    ;
DecList : Dec {}
        | Dec COMMA DecList {}
        ;
Dec : VarDec {}
    | VarDec ASSIGNOP Exp {}
    ;
/* Expressions */
Exp : Exp ASSIGNOP Exp { $$ = $1; }
    | Exp AND Exp {}
    | Exp OR Exp {}
    | Exp RELOP Exp {}
    | Exp PLUS Exp {}
    | Exp MINUS Exp {}
    | Exp STAR Exp {}
    | Exp DIV Exp {}
    | LP Exp RP {}
    | MINUS Exp {}
    | NOT Exp {}
    | ID LP Args RP {}
    | ID LP RP {}
    | Exp LB Exp RB {}
    | Exp DOT ID {}
    | ID {}
    | INT {}
    | FLOAT {}
    ;
Args : Exp COMMA Args {}
     | Exp {}
     ;
%%
#include "lex.yy.c"
void yyerror(char* msg) {
    printf("Error type B at Line %d: %s.\n", yylineno, msg);
}
