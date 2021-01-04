#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

double calc(const char* eqBegin, int eqLength, double calcPoint);

void return_char(int c, char **inp);
int read_char(char **inp);

double read_value(char **inp, double xValue);

double expression(char **inp, double xValue);
double ingredient(char **inp, double xValue);
double factor(char **inp, double xValue);
double exponent(char **inp, double xValue);