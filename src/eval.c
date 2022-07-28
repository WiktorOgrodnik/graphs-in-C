#include "eval.h"

#ifdef PARSER_INCLUDED
    #include "graphs-in-C-core/src/eval.c" 
#else
    double eval(Expr* expression, double xValue, int* error, char* message) {
        return 0.0;
    }

#endif