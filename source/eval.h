#ifndef EVAL_H
#define EVAL_H

#include "types.h"

//Calculate the function at point 'calcPoint' 
double calc(const char* eqBegin, int eqLength, double calcPoint, int* error, char* message); 
// eqBegin = pointer to equation string, eqLength = Lenght of egation string, calcPoint = x, stop = True if equation is undefined

#endif