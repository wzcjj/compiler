#ifndef COMPILER_SYNTAX_TREE_H
#define COMPILER_SYNTAX_TREE_H

//Copyright © 2017 wzcjj, Nanjing university

#include "list.h"
#include <string.h>

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

#define isSyntax(p, token) \
    ((p) != NULL && strcmp((p)->name, #token) == 0)

#define getChilds(node) \
    TreeNode *childs[10]; \
    int childscnt = 0; \
    do { \
        List *pointer; \
        listForeach(pointer, &(node)->child) { \
            childs[++childscnt] = listEntry(pointer, TreeNode); \
        } \
    } while (0)

#endif
