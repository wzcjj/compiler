#include <stdlib.h>
#include "syntax_tree.h"

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
