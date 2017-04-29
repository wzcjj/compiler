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
        "Redefined field \"%s\"",
        "Duplicated name \"%s\"",
        "Undefined structure \"%s\"",
        "Undefined function \"%s\"",
        "Inconsistent declaration of function \"%s\"",
};

typedef struct Val {
    Type *type;
    bool isvar;
} Val;
static Type *rettype;
static void analyseExtDefList(TreeNode*);
static void analyseExtDef(TreeNode*);
static void analyseExtDecList(TreeNode*, Type*);
static Type *analyseSpecifier(TreeNode*);
static Type *analyseStructSpecifier(TreeNode*);
static void analyseOptTag(TreeNode*, Type*);
static Type *analyseTag(TreeNode*);
static Field *analyseVarDec(TreeNode*, Type*);
static Symbol *analyseFunDec(TreeNode*, Type*, bool);
static void analyseVarList(TreeNode*, Args*);
static Arg *analyseParamDec(TreeNode*);
static void analyseCompSt(TreeNode*, Func*);
static void analyseStmtList(TreeNode*);
static void analyseStmt(TreeNode*);
static void analyseDefList(TreeNode*, Fields*);
static void analyseDef(TreeNode*);
static void analyseDecList(TreeNode*, Type*);
static void analyseDec(TreeNode*, Type*);
static Val analyseExp(TreeNode*);
static void analyseArgs(TreeNode*);

static Val requireType(TreeNode*, Type*, int);

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

static Field *analyseVarDec(TreeNode *p, Type *type) {
    getChilds(p);
    if (isSyntax(childs[1], ID)) {
        Field *dec = (Field*) malloc(sizeof(Field));
        dec->name = toArray(childs[1]->text);
        dec->type = type;
        return dec;
    }
    else {
        Type *newtype = (Type*) malloc(sizeof(Type));
        newtype->kind = ARRAY;
        newtype->array.elem = type;
        newtype->array.size = childs[3]->intval;
        return analyseVarDec(childs[1], newtype);
    }
}

static Symbol *analyseFunDec(TreeNode *p, Type *type, bool isdef) {
    getChilds(p);
    Func *func = newFunc(type);
    Symbol *symbol = symbolFind(childs[1]->text);
    if (symbol != NULL && (symbol->kind != FUNC ||
                           (isdef && symbol->func->defined))) {
        semanticError(4, childs[1]->lineno, symbol->name);
    }
    else {
        if (childscnt == 4) analyseVarList(childs[3], &func->args);
        if (symbol == NULL) {
            symbol = newFuncSymbol(childs[1]->text, func);
            symbolInsert(symbol);
        }
        else {
            if (funcEqual(symbol->func, func)) {
                funcRelease(func);
                return symbol;
            }
            else {
                semanticError(19, p->lineno, symbol->name);
            }
        }
    }
    funcRelease(func);
    return NULL;
}

static void analyseVarList(TreeNode *p, Args *args) {
    getChilds(p);
    Arg *arg = analyseParamDec(childs[1]);
    listAddBefore(args, &arg->list);
    if (childscnt == 3) analyseVarList(childs[3], args);
}

static Arg *analyseParamDec(TreeNode *p) {
    getChilds(p);
    Type *type = analyseSpecifier(childs[1]);
    return analyseVarDec(childs[3], type);
}

static void analyseCompSt(TreeNode *p, Func *func) {
    getChilds(p);
    symbolStackPush();
    if (func != NULL) {
        List *q;
        listForeach(q, &func->args) {
            Arg *arg = listEntry(q, Arg);
            Symbol *symbol = newVarSymbol(arg->name, arg->type);
            if (!symbolInsert(symbol))
                semanticError(3, p->lineno, symbol->name);
        }
    }
    analyseDefList(childs[2], NULL);
    analyseStmtList(childs[3]);
    symbolStackPop();
}

static void analyseStmtList(TreeNode *p) {
    if (p == NULL) return;
    getChilds(p);
    analyseStmt(childs[1]);
    analyseStmtList(childs[2]);
}

static void analyseStmt(TreeNode *p) {
    getChilds(p);
    if (isSyntax(childs[1], Exp)) {
        analyseExp(childs[1]);
    }
    else if (isSyntax(childs[1], CompSt)) {
        analyseCompSt(childs[1], NULL);
    }
    else if (isSyntax(childs[1], RETURN)) {
        Type *type = analyseExp(childs[2]).type;
        if (typeEqual(type, rettype)) semanticError(8, p->lineno, "");
    }
    else {
        requireType(childs[3], TYPE_INT, 7);
        analyseStmt(childs[5]);
        if (childscnt == 7) analyseStmt(childs[7]);
    }
}
