//Copyright © 2017 wzcjj, Nanjing university
#include <stdlib.h>
#include <string.h>
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
        SymbolNode *p = listEntry3(top->next, SymbolNode, stack);
        listDelete(&p->list);
        listDelete(&p->stack);
        symbolRelease(p->symbol);
        free(p);
    }
    stacktop--;
}

Symbol *symbolFind(const char *name) {
    unsigned val = hashPJW(name);
    List *p;
    listForeach(p, symboltable + val) {
        Symbol *symbol = listEntry(p, SymbolNode)->symbol;
        if (strcmp(symbol->name, name) == 0) return symbol;
    }
    return NULL;
}

bool symbolAtStackTop(const char *name) {
    Symbol *symbol = symbolFind(name);
    return (symbol != NULL) && (symbol->depth == stacktop);
}

bool symbolInsert(Symbol *symbol) {
    if (symbolAtStackTop(symbol->name)) return false;
    SymbolNode *p = (SymbolNode*) malloc(sizeof(SymbolNode));
    symbol->depth = stacktop;
    p->symbol = symbol;
    listInit(&p->list);
    listInit(&p->stack);
    unsigned val = hashPJW(symbol->name);
    listAddAfter(symboltable + val, &p->list);
    listAddBefore(stack + stacktop, &p->stack);
    return true;
}
