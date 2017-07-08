//Copyright © 2017 wzcjj, Nanjing university

#include "common.h"
#include "inter_code.h"

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

static void mipsRead();
static void mipsWrite();
static char *getreg(Operand*);

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

static void mipsDefLabel(Operand *label) {
    printf("label%d:\n",label->id);
}

static void mipsTranslate(InterCode *ic) {
    if (ic->kind == ADD) {
        if (ic->op1->kind == CONSTANT) printf("li");
        else printf("move");
        printf(MIPS[ic->kind], getreg(ic->res), getreg(ic->op1));
    }
}

void mipsMainLoop(InterCodes *Listhead) {
    List *p;
    listForeach(p, Listhead) {
        InterCode *q = listEntry(p, InterCode);
        mipsTranslate(q);
    }
}

static char *getreg(Operand *op) {
    return "";
}
