#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"
#include "symbol.h"

static Type TYPE_INT_, TYPE_FLOAT_;
Type *const TYPE_INT = &TYPE_INT_;
Type *const TYPE_FLOAT = &TYPE_FLOAT_;

void typesInit() {
    TYPE_INT_.kind = BASIC;
    TYPE_INT_.basic = 0;
    TYPE_FLOAT_.kind = BASIC;
    TYPE_FLOAT_.basic = 1;
}

bool typeEqual(Type *a, Type *b) {
    Assert(a != NULL);
    Assert(b != NULL);
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
    Assert(a != NULL);
    Assert(b != NULL);
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
    Assert(a != NULL);
    Assert(b != NULL);
    return typeEqual(a->rettype, b->rettype) && argsEqual(&a->args, &b->args);
}

void typeRelease(Type *type) {
    Assert(type != NULL);
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
    Assert(args != NULL);
    while (!listIsEmpty(args)) {
        Arg *arg = listEntry(args->next, Arg);
        listDelete(&arg->list);
        free(arg->name);
        free(arg);
    }
}

void funcRelease(Func *func) {
    Assert(func != NULL);
    argsRelease(&func->args);
    free(func);
}

Field *fieldFind(Fields *structure, const char *fieldname) {
    Assert(structure != NULL);
    Assert(fieldname != NULL);
    List *p;
    listForeach(p, structure) {
        Field *field = listEntry(p, Field);
        if (strcmp(field->name, fieldname) == 0) return field;
    }
    return NULL;
}

static Type *baseType(Type *type) {
    Assert(type != NULL);
    if (type->kind != ARRAY) return type;
    else return baseType(type->array.elem);
}

static void typeArrayToStr(Type *type, char *s) {
    Assert(type != NULL);
    if (type->kind == ARRAY) {
        sprintf(s, "[%d]", type->array.size);
        s += strlen(s);
        typeArrayToStr(type->array.elem, s);
    }
}

static void typeToStr(Type *type, char *s) {
    Assert(type != NULL);
    if (typeEqual(type, TYPE_INT)) strcpy(s, "int");
    else if (typeEqual(type, TYPE_FLOAT)) strcpy(s, "float");
    else if (type->kind == STRUCTURE) strcpy(s, "struct");
    else {
        typeToStr(baseType(type), s);
        s += strlen(s);
        typeArrayToStr(type, s);
    }
}

void argsToStr(Args *list, char *s) {
    Assert(list != NULL);
    Args *p;
    listForeach(p, list) {
        Arg *arg = listEntry(p, Arg);
        if (p != list->next) {
            strcpy(s, ", ");
            s += 2;
        }
        typeToStr(arg->type, s);
        s += strlen(s);
    }
    *s = 0;
}

int typeSize(Type *type) {
    Assert(type != NULL);
    int size = 0;
    List *p;
    switch (type->kind) {
    case BASIC:
        return 4;
    case ARRAY:
        return typeSize(type->array.elem) *type->array.size;
    case STRUCTURE:
        listForeach(p, &type->structure) {
            Field *field = listEntry(p, Field);
            size += typeSize(field->type);
        }
        return size;
    }
    return 0;
}

int fieldOffest(Fields *structure, const char *fieldname) {
    Assert(structure != NULL);
    Assert(fieldname != NULL);
    int offest = 0;
    List *p;
    listForeach(p, structure) {
        Field *field = listEntry(p, Field);
        if (strcmp(field->name, fieldname) == 0) return offest;
        offest += typeSize(field->type);
    }
    return -1;
}
