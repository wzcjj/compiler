#include <stdio.h>
extern FILE* yyin;
int main(int argc, char** argv) {
    if (argc <= 2) return 1;
    FILE *fin = fopen(argv[1], "r");
    FILE *fout = fopen(argv[2], "w");
    if (!fin) {
        perror(argv[1]);
        return 1;
    }
    yyrestart(f);
    yyparse();
    return 0;
}
