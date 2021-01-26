#ifndef EVAL_H
#define EVAL_H

#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

extern void error_dialog(const char* message);

double calc(const char* eqBegin, int eqLength, double calcPoint, bool* stop);

#endif