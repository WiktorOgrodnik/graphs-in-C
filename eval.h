#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

double calc(const char* eqBegin, int eqLength, double calcPoint, bool* dziedzina);

double eval(char* begin, char* end, double xValue, bool* dziedzina);
char findMsign(char* begin, char* end, char** position);
char* searchF(char* begin, char* end);

void segfault(char info []);