//Copyright © 2017 wzcjj, Nanjing university
#include <stdlib.h>
#include "symbol.h"

#define MASK 0x3fff
#define SIZE (MASK + 1)

typedef struct SymbolNode {
    Symbol *symbol;
    List list, stack;
} SymbolNode;

static List symboltable[SIZE], stack[SIZE];
unsigned stacktop;

void symbolTableInit() {
    int i;
    stacktop = 0;
    for (i = 0; i < SIZE; i++) {
        listInit(symboltable + i);
        listInit(stack + i);
    }
}

static unsigned hashPJW(const char *name) {
    unsigned val = 0;
    for (; *name; name++) {
        val = (val << 2) + *name;
        unsigned i = val & ~MASK;
        if (i) val = (val ^ (i>>12)) & MASK;
    }
    return val;
}

void symbolRelease(Symbol *symbol) {
    SymbolKind kind = symbol->kind;
    Type *type = symbol->type;
    if (kind == STRUCT) typeRelease(type);
    free(symbol->name);
    free(symbol);
}

void symbolStackPush() {
    stacktop++;
}

void symbolStackPop() {
    List *top = stack + stacktop;
    while (!listIsEmpty(top)) {
        SymbolNode *p = listEntry(top->next, SymbolNode, stack);
        listDelete(&p->list);
        listDelete(&p->stack);
        symbolRelease(p->symbol);
        free(p);
    }
    stacktop--;
}
