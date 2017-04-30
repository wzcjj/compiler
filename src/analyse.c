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
        listForeach(pointer, &(node)->child) { \
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
        "%s field \"%s\"",
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
static void analyseDef(TreeNode*, Fields*);
static void analyseDecList(TreeNode*, Type*, Fields*);
static void analyseDec(TreeNode*, Type*, Fields*);
static Val analyseExp(TreeNode*);
static void analyseArgs(TreeNode*, Args*);

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
            return symbol;
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
    return;
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
        if (!typeEqual(type, rettype)) semanticError(8, p->lineno, "");
    }
    else {
        requireType(childs[3], TYPE_INT, 7);
        analyseStmt(childs[5]);
        if (childscnt == 7) analyseStmt(childs[7]);
    }
}

static void analyseDefList(TreeNode *p, Fields *list) {
    if (p == NULL) return;
    getChilds(p);
    analyseDef(childs[1], list);
    analyseDefList(childs[2], list);
}

static void analyseDef(TreeNode *p, Fields *list) {
    getChilds(p);
    Type *type = analyseSpecifier(childs[1]);
    analyseDecList(childs[2], type, list);
}

static void analyseDecList(TreeNode *p, Type *type, Fields *list) {
    getChilds(p);
    analyseDec(childs[1], type, list);
    if (childscnt == 3) analyseDecList(childs[3], type, list);
}

static void analyseDec(TreeNode *p, Type *type, Fields *list) {
    getChilds(p);
    Field *dec = analyseVarDec(childs[1], type);
    if (list != NULL) {
        if (fieldFind(list, dec->name) != NULL)
            semanticError(15, p->lineno, "Redefined", dec->name);
        else listAddBefore(list, &dec->list);
        if (childscnt == 3)
            semanticError(15, p->lineno, "Initialized", dec->name);
    }
    else {
        Symbol *symbol = newVarSymbol(dec->name, dec->type);
        if (!symbolInsert(symbol))
            semanticError(3, p->lineno, symbol->name);
        else if (childscnt == 3) {
            Val val = analyseExp(childs[3]);
            if (!typeEqual(val.type, symbol->type))
                semanticError(5, childs[2]->lineno, "");
        }
        free(dec->name);
        free(dec);
    }
}

static Val makeVar(Type *type) {
    Val val;
    val.type = type;
    val.isvar = true;
    return val;
}

static Val makeVal(Type *type) {
    Val val;
    val.type = type;
    val.isvar = false;
    return val;
}

static Val requireBasic(TreeNode *p, int errorno) {
    Val val = analyseExp(p);
    if (val.type->kind != BASIC)
        semanticError(errorno, p->lineno, p->text);
    return val;
}

static Val requireType(TreeNode *p, Type *type, int errorno) {
    Val val = analyseExp(p);
    if (!typeEqual(val.type, type))
        semanticError(errorno, p->lineno, p->text);
    return val;
}

static Val analyseExp(TreeNode *p) {
    getChilds(p);
    if (isSyntax(childs[1], Exp)) {
        if (isSyntax(childs[2], ASSIGNOP)) {
            Val left = analyseExp(childs[1]);
            if (!left.isvar)
                semanticError(6,childs[1]->lineno, "");
            else {
                requireType(childs[3], left.type, 5);
                return left;
            }
        }
        else if (isSyntax(childs[2], AND) || isSyntax(childs[2], OR)) {
            requireType(childs[1], TYPE_INT, 7);
            requireType(childs[3], TYPE_INT, 7);
            return makeVal(TYPE_INT);
        }
        else if (isSyntax(childs[2], RELOP) || isSyntax(childs[2], PLUS)
                 || isSyntax(childs[2], MINUS) || isSyntax(childs[2], STAR)
                 || isSyntax(childs[2], DIV)) {
            Val left = requireBasic(childs[1], 7);
            requireType(childs[3], left.type, 7);
            return makeVal(left.type);
        }
        else if (isSyntax(childs[2], LB)) {
            Val base = analyseExp(childs[1]);
            requireType(childs[3], TYPE_INT, 12);
            if (base.type->kind != ARRAY)
                semanticError(10, childs[1]->lineno, childs[1]->text);
            else {
                base.type = base.type->array.elem;
                return base;
            }
        }
        else {
            Val base = analyseExp(childs[1]);
            char *fieldname = childs[3]->text;
            if (base.type->kind != STRUCTURE)
                semanticError(13, childs[2]->lineno, "");
            else {
                Field *field = fieldFind(&base.type->structure, fieldname);
                if (field == NULL)
                    semanticError(14, childs[3]->lineno, fieldname);
                else {
                    base.type = field->type;
                    return base;
                }
            }
        }
    }
    else if (isSyntax(childs[1], LP)) {
        return analyseExp(childs[2]);
    }
    else if (isSyntax(childs[1], MINUS)) {
        Val val = requireBasic(childs[2], 7);
        return makeVal(val.type);
    }
    else if (isSyntax(childs[1], NOT)) {
        requireType(childs[2], TYPE_INT, 7);
        return makeVal(TYPE_INT);
    }
    else if (isSyntax(childs[1], INT)) return makeVal(TYPE_INT);
    else if (isSyntax(childs[1], FLOAT)) return makeVal(TYPE_FLOAT);
    else {
        if (childscnt == 1) {
            Symbol *symbol = symbolFind(childs[1]->text);
            if (symbol == NULL)
                semanticError(1, childs[1]->lineno, childs[1]->text);
            else return makeVar(symbol->type);
        }
        else {
            Symbol *symbol = symbolFind(childs[1]->text);
            if (symbol == NULL)
                semanticError(2, childs[1]->lineno, childs[1]->text);
            else if (symbol->kind != FUNC)
                semanticError(11, childs[1]->lineno, childs[1]->text);
            else {
                Args args;
                listInit(&args);
                if (childscnt == 4) analyseArgs(childs[3], &args);
                if (!argsEqual(&args, &symbol->func->args)) {
                    char funcstr[32], argsstr[32];
                    argsToStr(&symbol->func->args, funcstr);
                    argsToStr(&args, argsstr);
                    semanticError(9, childs[1]->lineno, symbol->name,
                                  funcstr, argsstr);
                }
                argsRelease(&args);
                return makeVal(symbol->func->rettype);
            }
        }
    }
    return makeVal(TYPE_INT);
}

static void analyseArgs(TreeNode *p, Args *args) {
    getChilds(p);
    Arg *arg = (Arg*) malloc(sizeof(Arg));
    arg->type = analyseExp(childs[1]).type;
    arg->name = NULL;
    listAddBefore(args, &arg->list);
    if (childscnt == 3) analyseArgs(childs[3], args);
}
