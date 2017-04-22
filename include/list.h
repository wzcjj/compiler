#ifndef COMPILER_LIST_H
#define COMPILER_LIST_H

typedef struct List {
    struct List *prev, *next;
} List;

#define listEntry(ptr, type) \
    ((type*)((char*)(ptr) - (int)(&((typr*)0)->list)))

void listAdd(List*, List*, List*);

void listAddBefore(List*, List*);

void listAddAfter(List*, List*);

void listDelete(List*);

void listInit(List*);

#endif
