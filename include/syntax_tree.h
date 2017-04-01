#ifndef COMPILER_SYNTAX_TREE_H
#define COMPILER_SYNTAX_TREE_H

typedef struct TreeNode {
    int lineno, token;
    union {
        int intval;
        float floatval;
    };
    char *text, *name;
}TreeNode;

extern TreeNode *root;

TreeNode *newNode();

#endif
