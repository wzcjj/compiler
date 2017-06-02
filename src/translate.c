//Copyright © 2017 wzcjj, Nanjing university

#include "translate.h"
#include "symbol.h"
#include "common.h"
#include <stdlib.h>

static InterCodes head;
#define TRANSLATE_SIZE 10000
static List stack[TRANSLATE_SIZE];
static int top;

typedef struct InterCodeList {
    List list;
    InterCodes *head;
} InterCodeList;

void interCodesInit() {
    listInit(&head);
    listInit(stack);
    top = 0;
}

void interCodeStackPush() {
    Assert(++top < TRANSLATE_SIZE);
    listInit(stack + top);
}

void interCodeStackPop() {
    Assert(top-- > 0);
}

void interCodeStackInsert(InterCodes *listhead) {
    InterCodeList *listnode = (InterCodeList*) malloc(sizeof(InterCodeList));
    listAddBefore(stack + top, &listnode->list);
    listnode->head = listhead;
}

InterCodes *interCodeStackGet() {
    Assert(!listIsEmpty(stack + top));
    List *p = stack[top].next;
    InterCodeList *listnode = listEntry(p, InterCodeList);
    listDelete(p);
    InterCodes *listhead = listnode->head;
    free(listnode);
    return listhead;
}

InterCodes *interCodesGet() {
    return &head;
}

void defParams(Func *func) {
    List *p;
    listForeach(p, &func->args) {
        char *name = listEntry(p, Arg)->name;
        Symbol *symbol = symbolFind(name);
        symbol->id = newVarOperandId();
    }
}

void defFunc(char *name, InterCodes *irs) {
    Assert(name != NULL);
    Operand *op = newFuncOperand(name);
    InterCode *ir = newInterCode1(DEF_FUNCTION, op);
    interCodeInsert(&head, ir);
    if (listIsEmpty(irs) || listEntry(irs->prev, InterCode)->kind != RETURN) {
        ir = newInterCode1(RETURN, constOperand(-1));
        interCodeInsert(irs, ir);
    }
    interCodesBind(&head, irs);
}

static InterCodes *translateDefList(TreeNode*);
static InterCodes *translateDef(TreeNode*);
static InterCodes *translateDecList(TreeNode*);
static InterCodes *translateDec(TreeNode*);
static InterCodes *translateStmtList(TreeNode*);
static InterCodes *translateCond(TreeNode*, Operand*, Operand*);
static InterCodes *translateArgs(TreeNode*, List*, Args*);
static InterCodes *translateStmt(TreeNode*);

InterCodes *translateCompSt(TreeNode *p, Func *func) {
    getChilds(p);
    Assert(isSyntax(p, CompSt));
    InterCodes *irs = newInterCodes();
    if (func != NULL) {
        List *q;
        listForeach(q, &func->args) {
            char *name = listEntry(q, Arg)->name;
            Symbol *symbol = symbolFind(name);
            Operand *op = varOperand(symbol->id);
            InterCode *ir = newInterCode1(PARAM, op);
            interCodeInsert(irs, ir);
        }
    }
    if (isSyntax(childs[2], DefList))
        interCodesBind(irs, translateDefList(childs[2]));
    if (isSyntax(childs[childscnt - 1], StmtList))
        interCodesBind(irs, translateStmtList(childs[childscnt - 1]));
    return irs;
}

typedef struct OperandList {
    Operand *op;
    List list;
} OperandList;

static InterCodes *translateDefList(TreeNode *p) {
    Assert(isSyntax(p, DefList));
    getChilds(p);
    InterCodes *irs = translateDef(childs[1]);
    if (childscnt == 2) interCodesBind(irs, translateDefList(childs[2]));
    return irs;
}

static InterCodes *translateDef(TreeNode *p) {
    Assert(isSyntax(p, Def));
    getChilds(p);
    return translateDecList(childs[2]);
}

static InterCodes *translateDecList(TreeNode *p) {
    Assert(isSyntax(p, DecList));
    getChilds(p);
    InterCodes *irs = translateDec(childs[1]);
    if (childscnt == 3) interCodesBind(irs, translateDecList(childs[3]));
    return irs;
}

char *varDecToStr(TreeNode *p) {
    Assert(isSyntax(p, VarDec));
    getChilds(p);
    if (isSyntax(childs[1], ID)) return childs[1]->text;
    return varDecToStr(childs[1]);
}

static InterCodes *translateDec(TreeNode *p) {
    Assert(isSyntax(p, Dec));
    getChilds(p);
    Symbol *symbol = symbolFind(varDecToStr(childs[1]));
    InterCodes *irs = newInterCodes();
    Operand *var = symbolGetOperand(symbol);
    if (childscnt == 3) {
        Operand *op = newTempOperand();
        interCodesBind(irs, translateExp(childs[3], op));
        interCodeInsert(irs, newInterCode2(ASSIGN, var, op));
    }
    return irs;
}

static InterCodes *translateArgs(TreeNode *p, List *curarg, Args *args) {
    Assert(isSyntax(p, Args));
    getChilds(p);
    InterCodes *restirs = NULL;
    if (childscnt == 3)
        restirs = translateArgs(childs[3], curarg->next, args);
    Arg *arg = listEntry(curarg, Arg);
    Operand *op = newTempOperand();
    InterCodes *irs = translateExp(childs[1], op);
    if (restirs) irs = interCodesBind(restirs, irs);
    OperandList *operandnode = (OperandList*) malloc(sizeof(OperandList));
    operandnode->op = op;
    listAddBefore(args, &operandnode->list);
    return irs;
}

