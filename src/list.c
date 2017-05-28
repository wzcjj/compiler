#include <stdlib.h>
#include "list.h"
#include "common.h"

//Copyright © 2017 wzcjj, Nanjing university
static void listAdd(List *prev, List *next, List *node) {
    node->prev = prev;
    node->next = next;
    if (prev != NULL) prev->next = node;
    if (next != NULL) next->prev = node;
}

void listAddBefore(List *list, List *node) {
    listAdd(list->prev, list, node);
}

void listAddAfter(List *list, List *node) {
    listAdd(list, list->next, node);
}

void listDelete(List *node) {
    List *prev = node->prev, *next = node->next;
    if (prev != NULL) prev->next = next;
    if (next != NULL) next->prev = prev;
}

void listInit(List *list) {
    list->prev = list->next = list;
}

bool listIsEmpty(List *list) {
    return list == list->next;
}

void listMerge(List *first, List *second) {
    Assert(first != NULL);
    Assert(second != NULL);
    Assert(first != second);
    if (listIsEmpty(second)) return;
    first->prev->next = second->next;
    second->next->prev = first->prev;
    first->prev = second->prev;
    second->prev->next = first;
}
