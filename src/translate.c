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

static InterCodes *translateCond(TreeNode*, Operand*, Operand*);
static InterCodes *translateArgs(TreeNode*, List*, Args*);
static InterCodes *translateStmtList(TreeNode*);
static InterCodes *translateStmt(TreeNode*);
static InterCodes *translateDefList(TreeNode*);
static InterCodes *translateDecList(TreeNode*);
static InterCodes *translateDec(TreeNode*);


