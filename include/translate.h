//Copyright © 2017 wzcjj, Nanjing university
#ifndef COMPILER_TRANSLATE_H
#define COMPILER_TRANSLATE_H

#include "syntax_tree.h"
#include "inter_code.h"
#include "symbol.h"

void interCodeInit();
void interCodeStackPush();
void interCodeStackPop();
void interCodeStackInsert(InterCodes*);
InterCodes *interCodeStackGet();
InterCodes *interCodeGet();
InterCodes *translateExp(TreeNode*, Operand*);
InterCodes *translateCompSt(TreeNode*, Func*);
void defParams(Func*);
void defFunc(char*, InterCodes*);

#endif
