#include "parse.h"

#ifdef PARSER_INCLUDED
    #include "graphs-in-C-core/src/parse.c"
#else
    Expr* parse (const char* eqBegin, int* error, char* message) {
        *error = 6;
        return 0;
    }
#endif
