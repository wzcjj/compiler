#ifndef COMPILER_SYMBOL_H
#define COMPILER_SYMBOL_H
//Copyright © 2017 wzcjj, Nanjing university
#include "list.h"

typedef enum {
    BASIC, ARRAY, STRUCTURE
} TypeKind;
typedef List Fields;
typedef struct Type {
    TypeKind kind;
    union {
        int basic;
        struct {
            struct Type *elem;
            int size;
        } array;
        Fields structure;
    };
} Type;

typedef struct Field {
    char *name;
    Type *type;
    List list;
} Field;
typedef Fields Args;
typedef Field Arg;
typedef struct Func {
    Type *rettype;
    Args args;
    bool defined;
} Func;

extern Type *const TYPE_INT;
extern Type *const TYPE_FLOAT;

void typesInit();

bool typeEqual(Type*, Type*);
bool argsEqual(Args*, Args*);
bool funcEqual(Func*, Func*);

void argsRelease(Args*);
void funcRelease(Func*);
void typeRelease(Type*);

typedef enum {
    VAR, STRUCT, FUNC
} SymbolKind;
typedef struct Symbol {
    char *name;
    SymbolKind kind;
    union {
        Type *type;
        Func *func;
    };
    int id;
    bool isRef;
    int depth;
} Symbol;

#endif
