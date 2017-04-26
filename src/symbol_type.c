#include <string.h>
#include <stdlib.h>
#include "symbol.h"

static Type TYPE_INT_, TYPE_FLOAT_;
Type *const TYPE_INT = &TYPE_INT_;
Type *const TYPE_FLOAT = &TYPE_FLOAT_;

void typeInit() {
    TYPE_INT_.kind = BASIC;
    TYPE_INT_.basic = 0;
    TYPE_FLOAT_.kind = BASIC;
    TYPE_FLOAT_.basic = 1;
}

bool typeEqual(Type *a, Type *b) {
    if (a == b) return true;
    if (a->kind != b->kind) return false;
    List *p, *q;
    switch (a->kind) {
    case BASIC:
        return a->basic == b->basic;
    case ARRAY:
        return a->array.size == b->array.size
            && typeEqual(a->array.elem, b->array.elem);
    case STRUCTURE:
        p = a->structure.next;
        q = b->structure.next;
        while (p != &a->structure && q != &b->structure) {
            Type *typep = listEntry(p, Field)->type;
            Type *typeq = listEntry(q, Field)->type;
            if (!typeEqual(typep, typeq)) return false;
            p = p->next;
            q = q->next;
        }
        return p == &a->structure && q == &b->structure;
    }
}

bool argsEqual(Args *a, Args *b) {
    List *p = a->next, *q = b->next;
    while (p != a && q != b) {
        Arg *argp = listEntry(p, Arg);
        Arg *argq = listEntry(q, Arg);
        if (!typeEqual(argp->type, argq->type)) return false;
        p = p->next;
        q = q->next;
    }
    return p == a && q == b;
}

bool funcEqual(Func *a, Func *b) {
    return typeEqual(a->rettype, b->rettype) && argsEqual(&a->args, &b->args);
}

void typeRelease(Type *type) {
    if (type->kind == ARRAY) {
        typeRelease(type->array.elem);
        free(type);
    }
    else if (type->kind == STRUCTURE) {
        List *p;
        listForeach(p, &type->structure) {
            Field *field = listEntry(p, Field);
            typeRelease(field->type);
            free(field->name);
            free(field);
        }
        free(type);
    }
}

void argsRelease(Args *args) {
    while (!listIsEmpty(args)) {
        Arg *arg = listEntry(args->next, Arg);
        listDelete(&arg->list);
        free(arg->name);
        free(arg);
    }
}

void funcRelease(Func *func) {
    argsRelease(&func->args);
    free(func);
}

