#include "eval.h"
#define NUMBER 0

double calc(const char* eqBegin, int eqLength, double calcPoint)
{
    char begin[eqLength];

    for (int i = 0; i <= eqLength; i++)
    {
        begin[i] = *(eqBegin + i);
    }

    int c;
    double result;
    char* inptr = begin;
    
    if ((c = read_char(&inptr)) != EOF) 
    {
        return_char(c , &inptr);

        result = expression(&inptr, calcPoint);
    }

    return result;
}

void return_char(int c, char **inp)
{
    if (c != EOF && c != NUMBER) --*inp;
}

int read_char(char **inp)
{
    int c;
    
    if (**inp == '\0') return EOF;

    while ((c = *(*inp)++) != '\0' && isspace(c));

    if (isdigit(c) || c == '.' || c == ',' || isalpha(c)) 
    {
        return_char(c, inp);
        return NUMBER;
    }
    return c == 0 ? EOF : c;
}

double read_value(char **inp, double xValue)
{
    int c;
    double n = 0.0, exp10 = 1.0;

    while ((c = *(*inp)++) != '\0' && isdigit(c)) n = 10.0 * n + (c -'0');

    if (c == '.' || c == ',')
    {
        while ((c = *(*inp)++) != '\0' && isdigit(c)) 
        {
            n = 10.0 * n + (c - '0');
            exp10 *= 10.0;
        }
    }
    else if(c == 'x') 
    {
        n = xValue; 
        c = *(*inp)++;
    }
    else if (isalpha(c))
    {
        int it = 1;
        char function[10];
        function[0] = (char)c;

        while ((c = *(*inp)++) != '\0' && c != '(') 
        {
            function[it] = (char)c;
            it++;
        }
        function[it] = '\0';

        if (strcmp(function, "sin") == 0) n = sin(expression(inp, xValue));
        else if (strcmp(function, "cos") == 0) n = cos(expression(inp, xValue));
        else if (strcmp(function, "tan") == 0 || strcmp(function, "tg") == 0) n = tan(expression(inp, xValue));
        else if (strcmp(function, "cot") == 0 || strcmp(function, "ctg") == 0) n = 1 / tan(expression(inp, xValue));
        else if (strcmp(function, "log") == 0 || strcmp(function, "ln") == 0) n = log(expression(inp, xValue));
        else if (strcmp(function, "sqrt") == 0) n = sqrt(expression(inp, xValue));
        else return nan("ERROR!");

        if ((c = read_char(inp)) != ')') 
        {
            return nan("ERROR!");
            //Normalny blad
        }
        c = NUMBER;
    }

    return_char(c == 0 ? EOF : c, inp);        
    return n / exp10;
}

double expression(char **inp, double xValue)
{
    int c;
    double res, x2;

    if ((c = read_char(inp)) != '-' && c != '+') return_char(c, inp);

    res = ingredient(inp, xValue);

    if (c == '-') res = -res;

    while ((c = read_char(inp)) == '+' || c == '-') 
    {
        x2 = ingredient(inp, xValue);
        res = (c == '+' ? res + x2 : res - x2);
    }
    return_char(c, inp);        
    return res;
}

double ingredient(char **inp, double xValue)
{
    int c;
    double res, x2;

    res = factor(inp, xValue);
    while ((c = read_char(inp)) == '*' || c == '/') 
    {
        x2 = factor(inp, xValue);

        if (c == '*') res *= x2;
        else
        {
            if (x2) res /= x2;
            else return nan("ERROR!");
        }
    }

    return_char(c, inp);        
    return res;
}

double factor(char **inp, double xValue)
{
    int c;
    double res, x2;

    res = exponent(inp, xValue);
    while ((c = read_char(inp)) == '^') 
    {
        x2 = exponent(inp, xValue);

        if (res || x2) res = pow(res, x2);
        else return nan("ERROR!");
        //Uwaga na kolejność wykonywania działań!
        //Do poprawy
    }

    return_char(c, inp);        
    return res;
}

double exponent(char **inp, double xValue)
{
    int c;
    double res;
    //char error[100];

    if ((c = read_char(inp)) == NUMBER) return read_value(inp, xValue);
    else if (c == '(') 
    {
        res = expression(inp, xValue);

        if ((c = read_char(inp)) == ')') return res;
        else 
        {
	        //sprintf(blad,"BLAD: oczekiwano ')', a wystapil znak: '%c'\n",z);
	        //pokazBlad(blad);
            return nan("ERROR!");
        }
    }
    else 
    {
        //sprintf(blad,"BLAD: oczekiwano liczby lub '(', a wystapil znak: '%c'\n",z);
	    //pokazBlad(blad);
        return nan("ERROR!");
    }
}
