%{
#include "common.h"
#include <stdio.h>
#include <string.h>
#include "syntax_tree.h"
#define YYSTYPE TreeNode*
#include "syntax.tab.h"
#define handle(token) do { \
    yylval = newToken(token); \
    (*yylval).name = toArray(#token); \
    if (token == INT) (*yylval).intval = atoi(yytext); \
    else if (token == FLOAT) (*yylval).floatval = atof(yytext); \
    return token; \
}while(0)
TreeNode *newToken(int token) {
    TreeNode *p = newNode(token);
    (*p).lineno = yylineno;
    (*p).token = token;
    (*p).text = toArray(yytext);
    return p;
}
%}
%option yylineno
digit [0-9]
letter [a-zA-Z_]
space [ \t\n]
numHex 0[xX][0-9a-fA-F]+
numOct 0[0-7]+
numDec 0|([1-9]{digit}*)
id {letter}({letter}|{digit})*
num {numHex}|{numOct}|{numDec}
floatNormal {digit}+\.{digit}+
floatSci (({digit}*\.?{digit}+)|({digit}+\.))[eE][+-]?{digit}+
float {floatNormal}|{floatSci}
%%
>|<|>=|<=|==|!= { handle(RELOP); }
; { handle(SEMI); }
, { handle(COMMA); }
= { handle(ASSIGNOP); }
"+" { handle(PLUS); }
- { handle(MINUS); }
"*" { handle(STAR); }
"/" { handle(DIV); }
"&&" { handle(AND); }
"||" { handle(OR); }
"." { handle(DOT); }
"!" { handle(NOT); }
int|float { handle(TYPE); }
"(" { handle(LP); }
")" { handle(RP); }
"[" { handle(LB); }
"]" { handle(RB); }
"{" { handle(LC); }
"}" { handle(RC); }
struct { handle(STRUCT); }
return { handle(RETURN); }
if { handle(IF); }
else { handle(ELSE); }
while { handle(WHILE); }
{num} { /*printf("Integer value: %d\n", atoi(yytext));*/ handle(INT); }
{float} { /*printf("Float value: %f\n", atof(yytext));*/ handle(FLOAT); }
{id} { /*printf("Id name: %s\n", yytext);*/ handle(ID); }
{space} {}
"//" { while (1) { char c = input(); if(c == '\n')break; } }
"/*" {
    char c1 = 0, c2 = input();
    while (!((c1 == '*') && (c2 == '/'))) {
        c1 = c2;
        c2 = input();
        if (c2 < 0) break;
    }
}
. { printf("Error type A at line %d: Mysterious character \"%s\".\n", 
           yylineno, yytext); }
%%
