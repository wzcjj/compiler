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

void interCodeInit() {
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

InterCodes *interCodeGet() {
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
static InterCodes *translateStmt(TreeNode*);
static InterCodes *translateCond(TreeNode*, Operand*, Operand*);
static InterCodes *translateArgs(TreeNode*, List*, Args*);

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
    if (symbol->type->kind != BASIC) {
        InterCode *ir = newInterCode1(DEC, var);
        ir->size = typeSize(symbol->type);
        interCodeInsert(irs, ir);
    }
    if (childscnt == 3) {
        Operand *op = newTempOperand();
        interCodesBind(irs, translateExp(childs[3], op));
        interCodeInsert(irs, newInterCode2(ASSIGN, var, op));
    }
    return irs;
}

static InterCodes *translateStmtList(TreeNode *p) {
    Assert(isSyntax(p,StmtList));
    getChilds(p);
    InterCodes *irs = translateStmt(childs[1]);
    if (childscnt == 2) interCodesBind(irs, translateStmtList(childs[2]));
    return irs;
}

static InterCodes *translateStmt(TreeNode *p) {
    Assert(isSyntax(p, Stmt));
    getChilds(p);
    if (isSyntax(childs[1], Exp)) {
        return translateExp(childs[1], NULL);
    }
    else if (isSyntax(childs[1], CopmSt)) {
        return interCodeStackGet();
    }
    else if (isSyntax(childs[1], RETURN)) {
        Operand *op = newTempOperand();
        InterCodes *irs = translateExp(childs[2], op);
        InterCode *ir = newInterCode1(RETURN, op);
        interCodeInsert(irs, ir);
        return irs;
    }
    else if (isSyntax(childs[1], IF)) {
        bool el = childscnt == 7;
        TreeNode *stmt1, *stmt2;
        stmt1 = childs[5];
        if (el) stmt2 = childs[7];
        else stmt2 = NULL;
        Operand *label1 = newLabelOperand();
        Operand *label2 = newLabelOperand();
        Operand *label3 = NULL;
        InterCodes *irs = translateCond(childs[3], label1, label2);
        interCodeInsert(irs, newInterCode1(DEF_LABEL, label1));
        interCodesBind(irs, translateStmt(stmt1));
        if (stmt2) {
            label3 = newLabelOperand();
            interCodeInsert(irs, newInterCode1(GOTO, label3));
        }
        interCodeInsert(irs, newInterCode1(DEF_LABEL, label2));
        if (stmt2) {
            interCodesBind(irs, translateStmt(stmt2));
            interCodeInsert(irs, newInterCode1(DEF_LABEL, label3));
        }
        return irs;
    }
    else {
        Operand *label1 = newLabelOperand();
        Operand *label2 = newLabelOperand();
        Operand *label3 = newLabelOperand();
        InterCodes *irs = newInterCodes();
        interCodeInsert(irs, newInterCode1(DEF_LABEL, label1));
        interCodesBind(irs, translateCond(childs[3], label2, label3));
        interCodeInsert(irs, newInterCode1(DEF_LABEL, label2));
        interCodesBind(irs, translateStmt(childs[5]));
        interCodeInsert(irs, newInterCode1(GOTO, label1));
        return interCodeInsert(irs, newInterCode1(DEF_LABEL, label3));
    }
}

static InterCodes *translateCond(TreeNode *p, Operand *labeltrue,
                                 Operand *labelfalse) {
    Assert(isSyntax(p, Exp));
    getChilds(p);
    if (isSyntax(childs[1], NOT)) {
        return translateCond(childs[2], labelfalse, labeltrue);
    }
    else if (isSyntax(childs[2], RELOP)) {
        Operand *op1 = newTempOperand();
        Operand *op2 = newTempOperand();
        InterCodes *irs = translateExp(childs[1], op1);
        InterCodes *irs2 = translateExp(childs[3], op2);
        interCodesBind(irs, irs2);
        InterCode *ir = newInterCode3(GOTO_WITH_COND, labeltrue, op1, op2);
        ir->relop = childs[2]->text;
        interCodeInsert(irs, ir);
        return interCodeInsert(irs, newInterCode1(GOTO, labelfalse));
    }
    else if (isSyntax(childs[2], AND)) {
        Operand *label = newLabelOperand();
        InterCodes *irs = translateCond(childs[1], label, labelfalse);
        interCodeInsert(irs, newInterCode1(DEF_LABEL, label));
        return interCodesBind(irs, translateCond(childs[3], labeltrue, labelfalse));
    }
    else if (isSyntax(childs[2], OR)) {
        Operand *label = newLabelOperand();
        InterCodes *irs = translateCond(childs[1], labeltrue, label);
        interCodeInsert(irs, newInterCode1(DEF_LABEL, label));
        return interCodesBind(irs, translateCond(childs[3], labeltrue, labelfalse));
    }
    else {
        Operand *op = newTempOperand();
        InterCodes *irs = translateExp(p, op);
        InterCode *ir = newInterCode3(GOTO_WITH_COND, labeltrue, op, CONST_ZERO);
        ir->relop = "!=";
        interCodeInsert(irs, ir);
        ir = newInterCode1(GOTO, labelfalse);
        return interCodeInsert(irs, ir);
    }
}

InterCodes *translateExp(TreeNode *p, Operand *res) {
    Assert(isSyntax(p, Exp));
    getChilds(p);
    static Type *type = NULL;
    InterCodes *irs = newInterCodes();
    if (isSyntax(childs[1], ID)) {
        Symbol *symbol = symbolFind(childs[1]->text);
        if (childscnt == 1) {
            if (res == NULL) return irs;
            Operand *op = symbolGetOperand(symbol);
            type = symbol->type;
            if (res->id < 0 && type->kind != BASIC) {
                res->id = newTempOperandId();
                if (symbol->isref) {
                    interCodeInsert(irs, newInterCode2(ASSIGN, res, op));
                }
                else {
                    interCodeInsert(irs, newInterCode2(GET_REF, res, op));
                }
            }
            else {
                *res = *op;
            }
        }
        else {
            Assert(symbol->kind == FUNC);
            Args args;
            listInit(&args);
            if (isSyntax(childs[3], Args)) {
                List *firstarg = symbol->func->args.next;
                interCodesBind(irs, translateArgs(childs[3], firstarg, &args));
            }
            if (strcmp(symbol->name, "read") == 0) {
                if (!res) res = newTempOperand();
                InterCode *ir = newInterCode1(READ, res);
                interCodeInsert(irs, ir);
            }
            else if (strcmp(symbol->name, "write") == 0) {
                Operand *op = listEntry(args.next, OperandList)->op;
                InterCode *ir = newInterCode1(WRITE, op);
                interCodeInsert(irs, ir);
                if (res) {
                    op = constOperand(0);
                    ir = newInterCode2(ASSIGN, res, op);
                    interCodeInsert(irs, ir);
                }
            }
            else {
                List *q;
                listForeach(q, &args) {
                    Operand *op = listEntry(q, OperandList)->op;
                    InterCode *ir = newInterCode1(ARG, op);
                    interCodeInsert(irs,ir);
                }
                if (!res) res = newTempOperand();
                Operand *op = newFuncOperand(childs[1]->text);
                InterCode *ir = newInterCode2(CALL, res, op);
                interCodeInsert(irs,ir);
            }
            while (!listIsEmpty(&args)) {
                List *q = args.next;
                OperandList *operandnode = listEntry(q, OperandList);
                listDelete(q);
                free(operandnode);
            }
            type = symbol->func->rettype;
        }
    }
    else if (isSyntax(childs[1], INT)) {
        if (!res) return irs;
        *res = *constOperand(childs[1]->intval);
        type = TYPE_INT;
    }
    else if (isSyntax(childs[2], LB)) {
        Operand *index = newTempOperand();
        interCodesBind(irs, translateExp(childs[3], index));
        Operand *base = tempOperand(-1);
        interCodesBind(irs, translateExp(childs[1], base));
        Assert(type->kind == ARRAY);
        type = type->array.elem;
        Operand *offest = newTempOperand();
        Operand *size = constOperand(typeSize(type));
        interCodeInsert(irs, newInterCode3(MUL, offest, index, size));
        if (res->id < 0) {
            res->id = newTempOperandId();
            interCodeInsert(irs, newInterCode3(ADD, res, base, offest));
        }
        else {
            Operand *tmp = newTempOperand();
            interCodeInsert(irs, newInterCode3(ADD, tmp, base, offest));
            interCodeInsert(irs, newInterCode2(GET_ADDR, res, tmp));
        }
    }
    else if (isSyntax(childs[2], DOT)) {
        char *id = childs[3]->text;
        Operand *base = tempOperand(-1);
        interCodesBind(irs, translateExp(childs[1], base));
        Assert(type->kind == STRUCTURE);
        Field *field = fieldFind(&type->structure, id);
        Assert(field != NULL);
        Operand *offest = constOperand(fieldOffest(&type->structure, id));
        type = field->type;
        if (res->id < 0) {
            res->id = newTempOperandId();
            interCodeInsert(irs, newInterCode3(ADD, res, base, offest));
        }
        else {
            Operand *tmp = newTempOperand();
            interCodeInsert(irs, newInterCode3(ADD, tmp, base, offest));
            interCodeInsert(irs, newInterCode2(GET_ADDR, res, tmp));
        }
    }
    else if (isSyntax(childs[1], LP)) {
        free(irs);
        return translateExp(childs[2], res);
    }
    else if (isSyntax(childs[1], NOT) || isSyntax(childs[2], RELOP) ||
             isSyntax(childs[2], AND) || isSyntax(childs[2], OR)) {
        Operand *label1 = newLabelOperand();
        Operand *label2 = newLabelOperand();
        InterCodes *condirs = translateCond(p, label1, label2);
        if (!res) res = newTempOperand();
        interCodeInsert(irs, newInterCode2(ASSIGN, res, CONST_ZERO));
        interCodesBind(irs, condirs);
        interCodeInsert(irs, newInterCode1(DEF_LABEL, label1));
        interCodeInsert(irs, newInterCode2(ASSIGN, res, CONST_ONE));
        interCodeInsert(irs, newInterCode1(DEF_LABEL, label2));
        type = TYPE_INT;
    }
    else if (isSyntax(childs[1], MINUS)) {
        Operand *op = newTempOperand();
        InterCodes *expirs = translateExp(childs[2], op);
        interCodesBind(irs, expirs);
        if (!res) return irs;
        InterCode *ir = newInterCode3(SUB, res, CONST_ZERO, op);
        interCodeInsert(irs, ir);
    }
    else if (isSyntax(childs[2], ASSIGNOP)) {
        Operand *op1 = tempOperand(-1);
        Operand *op2 = newTempOperand();
        InterCodes *irs1 = translateExp(childs[1], op1);
        InterCodes *irs2 = translateExp(childs[3], op2);
        interCodesBind(irs, irs1);
        interCodesBind(irs, irs2);
        InterCode *ir = (op1->kind == VARIABLE) ?
            newInterCode2(ASSIGN, op1, op2) :
            newInterCode2(SET_ADDR, op1, op2);
        interCodeInsert(irs, ir);
        if (!res) return irs;
        if (res->id < 0) {
            *res = *op1;
        }
        else {
            interCodeInsert(irs, newInterCode2(GET_ADDR, res, op1));
        }
    }
    else {
        Operand *op1 = newTempOperand();
        Operand *op2 = newTempOperand();
        InterCodes *irs1 = translateExp(childs[1], op1);
        InterCodes *irs2 = translateExp(childs[3], op2);
        interCodesBind(irs, irs1);
        interCodesBind(irs, irs2);
        InterCodeKind kind = ADD;
        if (!res) return irs;
        if (isSyntax(childs[2], PLUS)) kind = ADD;
        else if (isSyntax(childs[2], MINUS)) kind = SUB;
        else if (isSyntax(childs[2], STAR)) kind = MUL;
        else if (isSyntax(childs[2], DIV)) kind = DIV;
        InterCode *ir = newInterCode3(kind, res, op1, op2);
        interCodeInsert(irs, ir);
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
    Operand *op = (arg->type->kind == BASIC) ? newTempOperand() : tempOperand(-1);
    InterCodes *irs = translateExp(childs[1], op);
    if (restirs) irs = interCodesBind(restirs, irs);
    OperandList *operandnode = (OperandList*) malloc(sizeof(OperandList));
    operandnode->op = op;
    listAddBefore(args, &operandnode->list);
    return irs;
}

