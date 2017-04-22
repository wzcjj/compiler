#include <stdlib.h>
#include "list.h"

//Copyright © 2017 wzcjj, Nanjing university
void listAdd(List *prev, List *next, List *node) {
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

