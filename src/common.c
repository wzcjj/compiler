#include <string.h>
#include <stdlib.h>

char *toArray(char *s) {
    char *p = malloc(strlen(s) + 1);
    strcpy(p, s);
    return p;
}
