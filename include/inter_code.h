#ifndef COMPILER_INTER_CODE_H
#define COMPILER_INTER_CODE_H

//Copyright © 2017 wzcjj, Nanjing university
#include "list.h"

// Operand
typedef enum {
    VARIABLE, TEMP, CONSTANT, LABEL, FUNCTION
} OperandKind;
typedef struct Operand {
    OperandKind kind;
    union {
        int id;
        int value;
        char *name;
    };
    char *text;
    char *reg;
} Operand;
extern Operand *const CONST_ZERO;
extern Operand *const CONST_ONE;

void operandInit();
Operand *newOperand(OperandKind);
int newVarOperandId();
int newTempOperandId();
int newLabelOperandId();
Operand *newVarOperand();
Operand *newTempOperand();
Operand *newLabelOperand();
Operand *newFuncOperand(char*);
Operand *constOperand(int);
Operand *varOperand(int);
Operand *tempOperand(int);
Operand *labelOperand(int);
char *operandToStr(Operand*);

// intercode
typedef enum {
    DEF_LABEL, DEF_FUNCTION, ASSIGN, ADD, SUB, MUL, DIV,
    GET_REF, GET_ADDR, SET_ADDR, GOTO, GOTO_WITH_COND,
    RETURN, DEC, ARG, CALL, PARAM, READ, WRITE
} InterCodeKind;
typedef struct InterCode {
    InterCodeKind kind;
    Operand *res, *op1, *op2;
    union {
        char *relop;
        int size;
    };
    List list;
} InterCode;

typedef List InterCodes;
InterCode *newInterCode3(InterCodeKind, Operand*, Operand*, Operand*);
InterCode *newInterCode2(InterCodeKind, Operand*, Operand*);
InterCode *newInterCode1(InterCodeKind, Operand*);
void interCodeToStr(InterCode*, char*);
void interCodesPrint(InterCodes*);
InterCodes *newInterCodes();
InterCodes *interCodeInsert(InterCodes*, InterCode*);
InterCodes *interCodesBind(InterCodes*, InterCodes*);
InterCodes *getInterCodesHead();

#endif
