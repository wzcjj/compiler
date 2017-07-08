#include <stdio.h>
#include "common.h"
#include "syntax_tree.h"
#include "translate.h"
#include "mips.h"
//Copyright © 2017 wzcjj, Nanjing university
extern FILE* yyin;
void yyrestart(FILE*);
void yyparse();
void analyseProgram(TreeNode*);
int errorstatus = 0;

void init() {
    symbolTableInit();
    typesInit();
    operandInit();
    interCodeInit();
    mipsInit();
}

int main(int argc, char** argv) {
    if (argc <= 2) return 1;
    FILE *fin = fopen(argv[1], "r");
    //FILE *fout = 
    freopen(argv[2], "w", stdout);
    if (!fin) {
        perror(argv[1]);
        return 1;
    }
    init();
    yyrestart(fin);
    yyparse();
    if (!errorstatus) {
        //print(root, 0);
        analyseProgram(root);
        interCodesPrint(interCodeGet());
    }
    return 0;
}
