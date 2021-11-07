#ifndef PARSE_H
#define PARSE_H

#include "types.h"

//Custom math functions
double fractionalPart (double a);
double cot (double a);

//Custom binary operators
double add (double a, double b);
double sub (double a, double b);
double mult (double a, double b);
double mdiv (double a, double b);

#define NUMBER_OF_FUNCTIONS 26
#define NUMBER_OF_CONSTANTS 3

// Array of functions
extern char* nameOfFunctions[NUMBER_OF_FUNCTIONS];
extern double (*functions[NUMBER_OF_FUNCTIONS])(double x);

// Array of constatns
extern char* nameOfConstants[NUMBER_OF_CONSTANTS];
extern double constants[NUMBER_OF_CONSTANTS];

// Type of expression node
enum ExprTag {
    CONST_EXPR,
    VAR_EXPR,
    BINOP_EXPR,
    FUN_EXPR,
    ERROR_EXPR
};

// Expression node structure
typedef struct ExprS {

    enum ExprTag tag;

    union {
        double data; // CONST_EXPR
        double (*binop)(double, double); // BINOP_EXPR
        double (*function)(double); // FUN_EXPR
        int error; // ERROR_EXPR;
    } val1;

    union {
        struct ExprS* left; // BINOP_EXPR, FUN_EXPR
        char* message; // ERROR_EXPR
    } val2;

    union {
        struct ExprS* right; // BINOP_EXPR
    } val3; 

} Expr;

//Parse string to mathematical expression
Expr* parse (const char* eqBegin, int* error, char* message); 

//Print parsed expression
char* to_string (Expr* expression);

#endif