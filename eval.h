#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

double calc(char* eqBegin, int eqLength, double calcPoint);

double eval(char* begin, char* end, double xValue);
char findMsign(char* begin, char* end, char** position);
char* searchF(char* begin, char* end);

void segfault(char info []);