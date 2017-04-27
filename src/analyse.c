#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "symbol.h"
#include "syntax_tree.h"

#define isSyntax(p, token) \
    ((p) != NULL && strcmp((p)->name, #token) == 0)
#define getChilds(node) \
TreeNode *childs[10]; \
    do { \
        List *pointer; \
        int childscnt = 0; \
        listForeach(pointer, &(node)->list) { \
            childs[++childscnt] = listEntry(pointer, TreeNode); \
        } \
    } while (0)


typedef Field Dec;
static void analyseExtDefList(TreeNode*);
static void analyseExtDef(TreeNode*);
static void analyseExtDecList(TreeNode*, Type*);
static Type *analyseSpecifier(TreeNode*);
static Type *analyseStructSpecifier(TreeNode*);
static Type *analyseOptTag(TreeNode*);
static Type *analyseTag(TreeNode*);
static Dec *analyseVarDec(TreeNode*, Type*);
static Symbol analyseFunDec(TreeNode*, Type*);
static void analyseVarList(TreeNode*);
static Arg analyseParamDec(TreeNode*);
static void analyseCompSt(TreeNode*);
static void analyseStmtList(TreeNode*);
static void analyseStmt(TreeNode*);
static void analyseDefList(TreeNode*);
static void analyseDef(TreeNode*);
static void analyseDecList(TreeNode*, Type*);
static void analyseDec(TreeNode*, Type*);
static void analyseExp(TreeNode*);
static void analyseArgs(TreeNode*);

void analyseProgram(TreeNode* p) {
    getChilds(p);
    symbolTableInit();
    typesInit();
    analyseExtDefList(childs[1]);
}

static void analyseExtDefList(TreeNode *p) {

}
