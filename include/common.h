#ifndef COMPILER_COMMON_H
#define COMPILER_COMMON_H

//Copyright © 2017 wzcjj, Nanjing university
#define ASSERT
#ifdef ASSERT
#include <assert.h>
#define Assert assert
#else
#define Assert(exp)
#endif
char *toArray(const char *);

extern int errorstatus;

/*enum {
    INT, ID, SEMI, COMMA, ASSIGNOP,
    RELOP, PLUS, MINUS, STAR, DIV,
    AND, OR, DOT, TYPE, LP,
    RP, LB, RB, LC, RC,
    STRUCT, RETURN, IF, ELSE, WHILE,
    NOT
};*/

#endif
