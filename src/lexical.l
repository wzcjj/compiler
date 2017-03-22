%{
#include<stdio.h>
#define handle(token) do { return token; }while(0)
%}
digit [0-9]
letter [a-zA-Z_]
space [ \t\n]
numHex 0[xX][0-9a-fA-F]+
numOct 0[0-7]+
numDec 0|([1-9]{digit}*)
id {letter}[{letter}{digit}]* 
num {numHex}|{numOct}|{numDec}
floatNormal {digit}+\.{digit}+
floatSci (({digit}*\.?{digit}+)|({digit}+\.))[eE][+-]?{digit}+
float {floatNormal}|{floatSci}
%%
{num} { printf("Integer value %d\n", atoi(yytext)); handle(INT); }
{id} { printf("Id name %s\n", yytext); handle(ID); }
; { printf("SEMI\n"); handle(SEMI); }
, { printf("COMMA\n"); handle(COMMA); }
= { printf("ASSIGNOP\n"); handle(ASSIGNOP); }
>|<|>=|<=|==|!= { printf("RELOP\n"); handle(RELOP); }
"+" { printf("PLUS\n"); handle(PLUS); }
- { printf("MINUS\n"); handle(MINUS); }
"*" { printf("MUL\n"); handle(STAR); }
"/" { printf("DIV\n"); handle(DIV); }
"&&" { printf("AND\n"); handle(AND); }
"||" { printf("OR\n"); handle(OR); }
"." { printf("DOT\n"); handle(DOT); }
"!" { printf("NOT\n"); handle(NOT); }
int|float {printf("TYPE\n"); handle(TYPE); }
"(" { printf("LP\n"); handle(LP); }
")" { printf("RP\n"); handle(RP); }
"[" { printf("LB\n"); handle(LB); }
"]" { printf("RB\n"); handle(RB); }
"{" { printf("LC\n"); handle(LC); }
"}" { printf("RC\n"); handle(RC); }
struct { printf("STRUCT\n"); handle(STRUCT); }
return { printf("RETURN\n"); handle(RETURN); }
if { printf("IF\n"); handle(IF); }
else { printf("ELSE\n"); handle(ELSE); }
while { printf("WHILE\n"); handle(WHILE); }
%%
