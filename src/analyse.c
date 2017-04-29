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
    int childscnt = 0; \
    do { \
        List *pointer; \
        listForeach(pointer, &(node)->list) { \
            childs[++childscnt] = listEntry(pointer, TreeNode); \
        } \
    } while (0)

#define semanticError(errorno, lineno, ...) \
    do { \
        errorstatus = 2; \
        printf("Error type %d at Line %d: ", (errorno), (lineno)); \
        printf(SEMANTIC_ERROR[(errorno) - 1], __VA_ARGS__); \
        printf(".\n"); \
    } while (0)
extern int errorstatus;
const char *SEMANTIC_ERROR[] = {
        "Undefined variable \"%s\"",
        "Undefined function \"%s\"",
        "Redefined variable \"%s\"",
        "Redefined function \"%s\"",
        "Type mismatched for assignment",
        "The left-hand side of an assignment must be a variable",
        "Type mismatched for operands",
        "Type mismatched for return",
        "Function \"%s(%s)\" is not applicable for arguments \"(%s)\"",
        "\"%s\" is not an array",
        "\"%s\" is not a function",
        "\"%s\" is not an integer",
        "Illegal use of \".\"",
        "Non-existent field \"%s\"",
        "Redefined or initialized field \"%s\"",
        "Duplicated name \"%s\"",
        "Undefined structure \"%s\"",
        "Undefined function \"%s\"",
        "Inconsistent declaration of function \"%s\"",
};

static void analyseExtDefList(TreeNode*);
static void analyseExtDef(TreeNode*);
static void analyseExtDecList(TreeNode*, Type*);
static Type *analyseSpecifier(TreeNode*);
static Type *analyseStructSpecifier(TreeNode*);
static void analyseOptTag(TreeNode*, Type*);
static Type *analyseTag(TreeNode*);
static void *analyseVarDec(TreeNode*, Type*);
static Symbol *analyseFunDec(TreeNode*, Type*, bool);
static void analyseVarList(TreeNode*);
static Arg analyseParamDec(TreeNode*);
static void analyseCompSt(TreeNode*, Func*);
static void analyseStmtList(TreeNode*);
static void analyseStmt(TreeNode*);
static void analyseDefList(TreeNode*, Fields*);
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
    if (p == NULL) return;
    getChilds(p);
    analyseExtDef(childs[1]);
    analyseExtDefList(childs[2]);
}

static void analyseExtDef(TreeNode *p) {
    getChilds(p);
    Type *type = analyseSpecifier(childs[1]);
    if (isSyntax(childs[2], ExtDecList)) {
        analyseExtDecList(childs[2], type);
    }
    else if (isSyntax(childs[2], FunDec)) {
        bool isdef = isSyntax(childs[3], CompSt);
        Symbol *symbol = analyseFunDec(childs[2], type, isdef);
        Func *func = symbol->func;
        if (isdef) {
            analyseCompSt(childs[3], func);
            func->defined = true;
        }
    }
}

static void analyseExtDecList(TreeNode *p, Type *type) {
    getChilds(p);
    analyseVarDec(childs[1], type);
    if (childscnt == 3) analyseExtDecList(childs[3], type);
}

static Type *analyseSpecifier(TreeNode *p) {
    getChilds(p);
    if (isSyntax(childs[1], TYPE)) {
        if (strcmp(childs[1]->text, "INT")) return TYPE_INT;
        else return TYPE_FLOAT;
    }
    else {
        return analyseStructSpecifier(childs[1]);
    }
}

static Type *analyseStructSpecifier(TreeNode *p) {
    getChilds(p);
    if (isSyntax(childs[2], Tag)) {
        return analyseTag(childs[2]);
    }
    else {
        Type *type = (Type*) malloc(sizeof(Type));
        type->kind = STRUCTURE;
        analyseDefList(childs[4], &type->structure);
        analyseOptTag(childs[2], type);
        return type;
    }
}

static void analyseOptTag(TreeNode *p, Type *type) {
    if (p == NULL) return;
    getChilds(p);
    Symbol *symbol = newStructSymbol(childs[1]->text, type);
    if (!symbolInsert(symbol))
        semanticError(16, childs[1]->lineno, childs[1]->text);
}

static Type *analyseTag(TreeNode *p) {
    getChilds(p);
    Symbol *symbol = symbolFind(childs[1]->text);
    if (symbol == NULL || symbol->kind != STRUCT) {
        semanticError(17, childs[1]->lineno, childs[1]->text);
        return TYPE_INT;
    }
    return symbol->type;
}
