#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "symbol.h"
#include "syntax_tree.h"

#define isSyntax(p, token) \
    ((p) != NULL && strcmp((p)->name, #token) == 0)
#define getChilds(node) \
    do { \
        List *pointer; \
        int childscnt = 0; \
        listForeach(pointer, &(node)->list) { \
            childs[++childscnt] = listEntry(pointer, TreeNode); \
        } \
    } while (0)

TreeNode *childs[10];

static void analyseExtDefList(TreeNode*);
static void analyseExtDef(TreeNode*);
static void analyseExtDecList(TreeNode*);
static void analyseSpecifier(TreeNode*);
static void analyseStructSpecifier(TreeNode*);
static void analyseOptTag(TreeNode*);
static void analyseTag(TreeNode*);
static void analyseVarDec(TreeNode*);
static void analyseFunDec(TreeNode*);
static void analyseVarList(TreeNode*);
static void analyseParamDec(TreeNode*);
static void analyseCompSt(TreeNode*);
static void analyseStmtList(TreeNode*);
static void analyseStmt(TreeNode*);
static void analyseDefList(TreeNode*);
static void analyseDef(TreeNode*);
static void analyseDecList(TreeNode*);
static void analyseDec(TreeNode*);
static void analyseExp(TreeNode*);
static void analyseArgs(TreeNode*);

void analyseProgram(TreeNode* p) {
    symbolTableInit();
    typesInit();
    getChilds(p);
    analyseExtDefList(childs[1]);
}

static void analyseExtDefList(TreeNode *p) {

}
