#ifndef COMPILER_SYMBOL_H
#define COMPILER_SYMBOL_H
//Copyright © 2017 wzcjj, Nanjing university
#include "list.h"
#include "inter_code.h"

//symbol_type
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

void argsToStr(Args*, char*);
Field *fieldFind(Fields *, const char *);

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
    int depth;
} Symbol;

// symbol_table
void symbolTableInit();
void symbolRelease(Symbol*);
void symbolStackPush();
void symbolStackPop();
Symbol *symbolFind(const char*);
bool symbolInsert(Symbol*);

// symbol
Symbol *newVarSymbol(const char*, Type*);
Symbol *newFuncSymbol(const char*, Func*);
Symbol *newStructSymbol(const char*, Type*);
Func *newFunc(Type*);
Operand *symbolGetOperand(Symbol);

#endif
