#ifndef COMPILER_LIST_H
#define COMPILER_LIST_H

//Copyright © 2017 wzcjj, Nanjing university
#include <stdbool.h>
typedef struct List {
    struct List *prev, *next;
} List;

#define listEntry(ptr, type) \
    ((type*)((char*)(ptr) - (int)(&((type*)0)->list)))

#define listEntry3(ptr, type, list) \
    ((type*)((char*)(ptr) - (int)(&((type*)0)->list)))

#define listForeach(p, head) \
    for ((p) = (head)->next; (p) != (head); (p) = (p)->next)

void listAddBefore(List*, List*);

void listAddAfter(List*, List*);

void listDelete(List*);

void listInit(List*);

bool listIsEmpty(List*);

#endif
