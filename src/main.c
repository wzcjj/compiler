#include <stdio.h>
extern FILE* yyin;
void yyrestart(FILE*);
void yyparse();
int main(int argc, char** argv) {
    if (argc <= 2) return 1;
    FILE *fin = fopen(argv[1], "r");
    FILE *fout = freopen(argv[2], "w", stdout);
    if (!fin) {
        perror(argv[1]);
        return 1;
    }
    yyrestart(fin);
    yyparse();
    return 0;
}
