//Copyright © 2017 wzcjj, Nanjing university

#include "common.h"
#include "inter_code.h"
#include <stdlib.h>

static const char *MIPS[] = {
    "%s:\n",
    "\n%s:\n",
    " %s, %s\n",
    " %s, %s, %s\n",
    " %s, %s, ",
    "  mul %s, %s, %s\n",
    "  div %s, %s\n  mflo %s\n",
    "",
    "  lw %s, 0(%s)\n",
    "  sw %s, 0(%s)\n",
    "  j %s",
    "",
    "  move $v0, %s\n  jr $ra\n",
    "",
    "",
    "  jal %s\n  move %s, $v0",
    "",
    "\
  addi $sp, $sp, -4\n\
  sw $ra, 0($sp)\n\
  jal read\n\
  lw $ra, 0($sp)\n\
  addi $sp, $sp, 4\n\
  move %s, $v0\n",
    "\
  addi $sp, $sp, -4\n\
  sw $ra, 0($sp)\n\
  jal write\n\
  lw $ra, 0($sp)\n\
  addi $sp, $sp, 4\n"
};

static char stringforconst[10];
static void mipsRead();
static void mipsWrite();
static char *getReg(Operand*);

void mipsInit() {
    printf("\
.data\n\
_prompt: .asciiz \"Enter an integer:\"\n\
_ret: .asciiz \"\\n\"\n\
.globl main\n\
.text\n");
    mipsRead();
    mipsWrite();
}

static void mipsRead() {
    printf("\
read:\n\
  li $v0, 4\n\
  la $a0, _prompt\n\
  syscall\n\
  li $v0, 5\n\
  syscall\n\
  jr $ra\n\n");
}

static void mipsWrite() {
    printf("\
write:\n\
  li $v0, 1\n\
  syscall\n\
  li $v0, 4\n\
  la $a0, _ret\n\
  syscall\n\
  move $v0, $0\n\
  jr $ra\n\n");
}

static void mipsTranslate(InterCode *ic) {
    Assert(ic != NULL);
    Operand *res = ic->res, *op1 = ic->op1, *op2 = ic->op2;
    int kind = ic->kind;
    if (kind == ASSIGN) {
        if (op1->kind == CONSTANT) printf("li");
        else printf("move");
        printf(MIPS[kind], getReg(res), getReg(op1));
    }
    else if (kind == ADD) {
        Assert(0);
        if (op2->kind == CONSTANT) printf("addi");
        else printf("add");
        printf(MIPS[kind], getReg(res), getReg(op1), getReg(op2));
    }
    else if (kind == SUB) {
        if (op2->kind == CONSTANT) printf("addi");
        else printf("sub");
        printf(MIPS[kind], getReg(res), getReg(op1));
        if (op2->kind == CONSTANT) printf("-");
        printf("%s\n", getReg(op2));
    }
    else if (kind == DIV) {
        printf(MIPS[kind], getReg(op1), getReg(op2), getReg(res));
    }
    else if (kind == SET_ADDR) {
        printf(MIPS[kind], getReg(op1), getReg(res));
    }
    else if (kind == CALL) {
        printf(MIPS[kind], getReg(op1), getReg(res));
    }
    else {
        printf(MIPS[kind], getReg(res), getReg(op1), getReg(op2));
    }
}

void mipsMainLoop() {
    InterCodes *Listhead = getInterCodesHead();
    List *p;
    listForeach(p, Listhead) {
        InterCode *q = listEntry(p, InterCode);
        mipsTranslate(q);
    }
}

static int regnum = 0;

static char *getReg(Operand *op) {
    if (op == NULL) return "";
    if (op->kind == LABEL || op->kind == FUNCTION) return operandToStr(op);
    else if (op->kind == CONSTANT) {
        sprintf(stringforconst, "%d", op->value);
        return stringforconst;
    }
    else {
        if (op->reg) {
            return op->reg;
        }
        else {
            op->reg = (char*) malloc(4);
            sprintf(op->reg, "$t%d", ++regnum);
            return op->reg;
        }
    }
    return "";
}
