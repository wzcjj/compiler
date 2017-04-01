#include <stdlib.h>
#include "syntax_tree.h"

TreeNode *newNode() {
    TreeNode *p = (TreeNode*) malloc(sizeof(TreeNode));
    return p;
}
