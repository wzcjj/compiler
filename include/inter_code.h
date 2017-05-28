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

#endif
