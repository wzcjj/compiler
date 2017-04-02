#ifndef COMPILER_SYNTAX_TREE_H
#define COMPILER_SYNTAX_TREE_H

//Copyright © 2017 wzcjj, Nanjing university
typedef struct TreeNode {
    int lineno, token;
    union {
        int intval;
        float floatval;
    };
    char *text, *name;
    struct TreeNode *child, *broprev, *bronext;
}TreeNode;

extern TreeNode *root;

TreeNode *newNode();
void treeAddChild(TreeNode*, TreeNode*);
void print(TreeNode*, int);

#endif
