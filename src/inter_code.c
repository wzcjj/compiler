//Copyright © 2017 wzcjj, Nanjing university
#include "inter_code.h"
#include "common.h"
#include <stdlib.h>
#include <stdio.h>

static InterCode irsPool[10000];

static const char *INTER_CODE[] = {
    "LABEL %s :",
    "FUNCTION %s :",
    "%s := %s",
    "%s := %s + %s",
    "%s := %s - %s",
    "%s := %s * %s",
    "%s := %s / %s",
    "%s := &%s",
    "%s := *%s",
    "*%s := %s",
    "GOTO %s",
    "",
    "RETURN %s",
    "",
    "ARG %s",
    "%s := CALL %s",
    "PARAM %s",
    "READ %s",
    "WRITE %s"
};

InterCode *newInterCode3(InterCodeKind kind, Operand *res, Operand *op1,
                         Operand *op2) {
    static int cnt = 0;
    InterCode *p = &irsPool[cnt++];
    p->kind = kind;
    p->res = res;
    p->op1 = op1;
    p->op2 = op2;
    return p;
}

InterCode *newInterCode2(InterCodeKind kind, Operand *res, Operand *op) {
    return newInterCode3(kind, res, op, NULL);
}

InterCode *newInterCode1(InterCodeKind kind, Operand *res) {
    return newInterCode3(kind, res, NULL, NULL);
}

InterCodes *newInterCodes() {
    InterCodes *irs = (InterCodes*) malloc(sizeof(InterCodes));
    listInit(irs);
    return irs;
}

InterCodes *interCodeInsert(InterCodes *head, InterCode *p) {
    Assert(head != NULL);
    Assert(p != NULL);
    listAddBefore(head, &p->list);
    return head;
}

InterCodes *interCodesBind(InterCodes *first, InterCodes *second) {
    Assert(first != NULL);
    Assert(second != NULL);
    Assert(first != second);
    listMerge(first, second);
    free(second);
    return first;
}

void interCodeToStr(InterCode *p, char *s) {
    Assert(p != NULL);
    Assert(s != NULL);
    if (p->kind == GOTO_WITH_COND) {
        sprintf(s, "IF %s %s %s GOTO %s", operandToStr(p->op1), p->relop,
                operandToStr(p->op2), operandToStr(p->res));
    }
    else if (p->kind == DEC) {
        sprintf(s, "DEC %s %d", operandToStr(p->res), p->size);
    }
    else {
        sprintf(s, INTER_CODE[p->kind], operandToStr(p->res),
                operandToStr(p->op1), operandToStr(p->op2));
    }
}

void interCodesPrint(InterCodes *head) {
    Assert(head != NULL);
    static char buf[120];
    List *p;
    listForeach(p, head) {
        InterCode *intercode = listEntry(p, InterCode);
        interCodeToStr(intercode, buf);
        printf("%s\n",buf);
    }
}

