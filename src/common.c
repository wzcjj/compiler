#include <string.h>
#include <stdlib.h>

//Copyright © 2017 wzcjj, Nanjing university
char *toArray(char *s) {
    char *p = malloc(strlen(s) + 1);
    strcpy(p, s);
    return p;
}
