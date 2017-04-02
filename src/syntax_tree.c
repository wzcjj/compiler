#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "syntax_tree.h"

//Copyright © 2017 wzcjj, Nanjing university
TreeNode *root = NULL;

TreeNode *newNode() {
    TreeNode *p = (TreeNode*) malloc(sizeof(TreeNode));
    (*p).child = NULL;
    (*p).broprev = (*p).bronext = p;
    return p;
}

void treeAddChild(TreeNode *root, TreeNode *p) {
    if ((*root).child == NULL) {
        (*root).child = p;
    }
    else {
        TreeNode *child = (*root).child;
        (*p).bronext = (*child).bronext;
        (*child).bronext = p;
        (*(*p).bronext).broprev = p;
        (*p).broprev = child;
    }
}

void print(TreeNode *now, int deep) {
    int i;
    for (i = 0; i < deep; i++) {
        printf("  ");
    }
    printf("%s", (*now).name);
    if ((*now).name[1] <= 'Z') {
        if (strcmp((*now).name, "ID") == 0
           || strcmp((*now).name, "TYPE") == 0) {
            printf(": %s", (*now).text);
        }
        else if (strcmp((*now).name, "INT") == 0) {
            printf(": %d", (*now).intval);
        }
        else if (strcmp((*now).name, "FLOAT") == 0) {
            printf(": %f", (*now).floatval);
        }
        //else printf("????");
    }
    else {
        printf(" (%d)", (*now).lineno);
    }
    printf("\n");
    TreeNode *child = (*now).child, *p;
    if (child != NULL) {
        print(child, deep + 1);
        for (p = (*child).broprev; p != child; p = (*p).broprev) {
            print(p, deep + 1);
        }
    }
}
