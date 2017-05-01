%{
#include "common.h"
#include <stdio.h>
#include <string.h>
#include "syntax_tree.h"
//Copyright © 2017 wzcjj, Nanjing university
int yylex();
#define YYTOKEN_TABLE 1
#define YYSTYPE TreeNode*
#define handle1(type) \
do {\
    int tokennum = yyr2[yyn]; \
    yyval = newNode(); \
    int i, len = 0; \
    for (i = 1; i <= tokennum; i++) { \
        TreeNode *p = yyvsp[i - tokennum]; \
        if (i == 1) yyval->lineno = p->lineno; \
        if (p != NULL) { \
            treeAddChild(yyval, p); \
            len += 1 + strlen(p->text); \
        } \
    } \
    List *q; \
    char *s = yyval->text = malloc(len); \
    listForeach(q, &yyval->child) { \
        TreeNode *p = listEntry(q, TreeNode); \
        if (q != yyval->child.next) *(s++) = ' '; \
        strcpy(s, p->text); \
        s += strlen(s); \
    } \
    yyval->name = (char*)(yytname[YYNTOKENS + type]); \
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
Program : ExtDefList { handle1(1); root = $$; }
        ;
ExtDefList : ExtDef ExtDefList { handle1(2); }
           | { $$ = NULL; }
           ;
ExtDef : Specifier ExtDecList SEMI { handle1(3); }
       | Specifier SEMI { handle1(3); }
       | Specifier FunDec CompSt { handle1(3); }
       | Specifier FunDec SEMI { handle1(3); }
       ;
ExtDecList : VarDec { handle1(4); }
           | VarDec COMMA ExtDecList { handle1(4); }
           ;
/* Specifiers */
Specifier : TYPE { handle1(5); }
          | StructSpecifier { handle1(5); }
          ;
StructSpecifier : STRUCT OptTag LC DefList RC { handle1(6); }
                | STRUCT Tag { handle1(6); }
                ;
OptTag : ID { handle1(7); }
       | { $$ = NULL; }
       ;
Tag : ID { handle1(8); }
    ;
/* Declarators */
VarDec : ID { handle1(9); }
       | VarDec LB INT RB { handle1(9); }
       ;
FunDec : ID LP VarList RP { handle1(10); }
       | ID LP RP { handle1(10); }
       ;
VarList : ParamDec COMMA VarList { handle1(11); }
        | ParamDec { handle1(11); }
        ;
ParamDec : Specifier VarDec { handle1(12); }
         ;
/* Statements */
CompSt : LC DefList StmtList RC { handle1(13); }
       ;
StmtList : Stmt StmtList { handle1(14); }
         | { $$ = NULL; }
         ;
Stmt : Exp SEMI { handle1(15); }
     | CompSt { handle1(15); }
     | RETURN Exp SEMI { handle1(15); }
     | IF LP Exp RP Stmt { handle1(15); }
     | IF LP Exp RP Stmt ELSE Stmt { handle1(15); }
     | WHILE LP Exp RP Stmt { handle1(15); }
     ;
/* Local Definitions */
DefList : Def DefList { handle1(16); }
        | { $$ = NULL; }
        ;
Def : Specifier DecList SEMI { handle1(17); }
    ;
DecList : Dec { handle1(18); }
        | Dec COMMA DecList { handle1(18); }
        ;
Dec : VarDec { handle1(19); }
    | VarDec ASSIGNOP Exp { handle1(19); }
    ;
/* Expressions */
Exp : Exp ASSIGNOP Exp { handle1(20); }
    | Exp AND Exp { handle1(20); }
    | Exp OR Exp { handle1(20); }
    | Exp RELOP Exp { handle1(20); }
    | Exp PLUS Exp { handle1(20); }
    | Exp MINUS Exp { handle1(20); }
    | Exp STAR Exp { handle1(20); }
    | Exp DIV Exp { handle1(20); }
    | LP Exp RP { handle1(20); }
    | MINUS Exp { handle1(20); }
    | NOT Exp { handle1(20); }
    | ID LP Args RP { handle1(20); }
    | ID LP RP { handle1(20); }
    | Exp LB Exp RB { handle1(20); }
    | Exp DOT ID { handle1(20); }
    | ID { handle1(20); }
    | INT { handle1(20); }
    | FLOAT { handle1(20); }
    ;
Args : Exp COMMA Args { handle1(21); }
     | Exp { handle1(21); }
     ;
%%
#include "lex.yy.c"
void yyerror(char* msg) {
    printf("Error type B at Line %d: %s.\n", yylineno, msg);
    errorstatus = 2;
}
