#include <stdlib.h>
#include "common.h"
#include "symbol.h"

static Symbol *newSymbol(SymbolKind kind, const char *name) {
    Symbol *symbol = (Symbol*) malloc(sizeof(Symbol));
    symbol->kind = kind;
    symbol->name = toArray(name);
    return symbol;
}
