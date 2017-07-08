//Copyright © 2017 wzcjj, Nanjing university

#include "common.h"
#include "inter_code.h"
#include <string.h>
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
    "  j %s\n",
    " %s, %s, %s\n",
    "  move $v0, %s\n  jr $ra\n",
    "",
    "",
    "  jal %s\n  move %s, $v0\n",
    "",
    "\
  addi $sp, $sp, -4\n\
  sw $ra, 0($sp)\n\
  jal read\n\
  lw $ra, 0($sp)\n\
  addi $sp, $sp, 4\n\
  move %s, $v0\n",
    "\
  move $a0, %s\n\
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
  jr $ra\n");
}

static void mipsTranslate(InterCode *ic) {
    Assert(ic != NULL);
    Operand *res = ic->res, *op1 = ic->op1, *op2 = ic->op2;
    int kind = ic->kind;
    if (kind == ASSIGN) {
        if (op1->kind == CONSTANT) printf("  li");
        else printf("  move");
        printf(MIPS[kind], getReg(res), getReg(op1));
    }
    else if (kind == ADD) {
        if (op2->kind == CONSTANT) printf("  addi");
        else printf("  add");
        printf(MIPS[kind], getReg(res), getReg(op1), getReg(op2));
    }
    else if (kind == SUB) {
        if (op2->kind == CONSTANT) printf("  addi");
        else printf("  sub");
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
    else if (kind == GOTO_WITH_COND) {
        char *relop = ic->relop;
        printf("  ");
        if (strcmp(relop, "==") == 0) printf("beq");
        else if (strcmp(relop, "!=") == 0) printf("bne");
        else if (strcmp(relop, ">") == 0) printf("bgt");
        else if (strcmp(relop, "<") == 0) printf("blt");
        else if (strcmp(relop, ">=") == 0) printf("bge");
        else if (strcmp(relop, "<=") == 0) printf("ble");
        else Assert(0);
        printf(MIPS[kind], getReg(op1), getReg(op2), getReg(res));
    }
    else if (kind == RETURN) {
        if (res->kind == CONSTANT) {
            printf("  li $t1, %s\n",getReg(res));
            printf(MIPS[kind], "$t1");
        }
        else printf(MIPS[kind], getReg(res));
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
static Operand *ops[10];
static char *regs[] = {"$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$t8"}; 

bool opEqual(Operand *op1, Operand *op2) {
    if (op1->kind != op2->kind) return false;
    return op1->id == op2->id;
}

static char *getReg(Operand *op) {
    if (op == NULL) return "";
    if (op->kind == LABEL || op->kind == FUNCTION) return operandToStr(op);
    else if (op->kind == CONSTANT) {
        sprintf(stringforconst, "%d", op->value);
        return stringforconst;
    }
    else {
        int i;
        for (i = 1; i <= regnum; i++)
            if (opEqual(ops[i], op)) return regs[i];
        ops[++regnum] = op;
        return regs[regnum];
    }
    return "";
}
