#ifndef COMPILER_SYNTAX_TREE_H
#define COMPILER_SYNTAX_TREE_H

//Copyright © 2017 wzcjj, Nanjing university

#include "list.h"

typedef struct TreeNode {
    int lineno, token;
    union {
        int intval;
        float floatval;
    };
    char *text, *name;
    List child, list;
}TreeNode;

extern TreeNode *root;

TreeNode *newNode();
void treeAddChild(TreeNode*, TreeNode*);
void print(TreeNode*, int);

#endif
