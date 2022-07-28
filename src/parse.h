#ifndef PARSE_H
#define PARSE_H

#ifdef PARSER_INCLUDED
    #include "graphs-in-C-core/src/eval.h"
#else
    // Expression node structure
    typedef struct Expr {
        char not_implemented;
    } Expr;
    Expr* parse (const char* eqBegin, int* error, char* message); 
#endif

#endif