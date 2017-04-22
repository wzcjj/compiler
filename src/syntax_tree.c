#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "syntax_tree.h"

//Copyright © 2017 wzcjj, Nanjing university
TreeNode *root = NULL;

TreeNode *newNode() {
    TreeNode *p = (TreeNode*) malloc(sizeof(TreeNode));
    listInit(&p->child);
    listInit(&p->list);
    return p;
}

void treeAddChild(TreeNode *now, TreeNode *p) {
    listAddBefore(&now->child, &p->list);
}

void print(TreeNode *now, int deep) {
    int i;
    for (i = 0; i < deep; i++) {
        printf("  ");
    }
    printf("%s", now->name);
    if (now->name[1] <= 'Z') {
        if (strcmp(now->name, "ID") == 0
           || strcmp(now->name, "TYPE") == 0) {
            printf(": %s", now->text);
        }
        else if (strcmp(now->name, "INT") == 0) {
            printf(": %d", now->intval);
        }
        else if (strcmp(now->name, "FLOAT") == 0) {
            printf(": %f", now->floatval);
        }
    }
    else {
        printf(" (%d)", now->lineno);
    }
    printf("\n");
    List *p;
    listForeach(p, &now->child) {
        print(listEntry(p, TreeNode), deep + 1);
    }
}
