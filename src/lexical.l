digit [0-9]
letter [a-zA-Z_]
space [ \t\n]
numHex 0[xX][0-9a-fA-F]+
numOct 0[0-7]+
numDec 0|([1-9]{digit}*)
num {numHex}|{numOct}|{numDec}
floatNormal {digit}+\.{digit}+
floatSci (({digit}*\.?{digit}+)|({digit}+\.))[eE][+-]?{digit}+
float {floatNormal}|{floatSci}
%%
{digit}+ { printf("Integer value %d\n", atoi(yytext)); }
{letter}[{letter}{digit}]* { printf("Id name %s\n", yytext); }
; { printf("SEMI\n"); }
, { printf("COMMA\n"); }
= { printf("ASSIGNOP\n"); }
>|<|>=|<=|==|!= { printf("RELOP\n"); }
"+" { printf("PLUS\n"); }
- { printf("MINUS\n"); }
"*" { printf("MUL\n"); }
"/" { printf("DIV\n"); }
"&&" { printf("AND\n"); }
"||" { printf("OR\n"); }
"." { printf("DOT\n"); }
"!" { printf("NOT\n"); }
int|float {printf("TYPE\n"); }
"(" { printf("LP\n"); }
")" { printf("RP\n"); }
"[" { printf("LB\n"); }
"]" { printf("RB\n"); }
"{" { printf("LC\n"); }
"}" { printf("RC\n"); }
struct { printf("STRUCT\n"); }
return { printf("RETURN\n"); }
if { printf("IF\n"); }
else { printf("ELSE\n"); }
while { printf("WHILE\n"); }
%%
