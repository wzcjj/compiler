digit [0-9]
letter [a-zA-Z]
letter_ [a-zA-Z_]
%%
{digit}+ { printf("Integer value %d\n", atoi(yytext)); }
{letter_}[{letter_}{digit}]* { printf("Id name %s\n", yytext); }
"+" { printf("PLUS\n"); }
"-" { printf("SUB\n"); }
"*" { printf("MUL\n"); }
"/" { printf("DIV\n"); }
"%" { printf("MOD\n"); }
"&&" { printf("AND\n"); }
"||" { printf("OR\n"); }
"!" { printf("NOT\n"); }
"^" { printf("XOR\n"); }
"&" { printf("B_AND\n"); }
"|" { printf("B_OR\n"); }
"~" { printf("B_NOT\n"); }

%%
