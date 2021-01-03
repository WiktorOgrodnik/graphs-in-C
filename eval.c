#include "eval.h"

double calc(const char* eqBegin, int eqLength, double calcPoint, bool* dziedzina)
{
    char begin[eqLength];

    for (int i = 0; i <= eqLength; i++)
    {
        begin[i] = *(eqBegin + i);
    }

    return (double)eval(begin, begin + eqLength, calcPoint, dziedzina);
}

double eval(char* begin, char* end, double xValue, bool* dziedzina)
{
    if(*begin == '(')
    {
        char* position = NULL;
        char t = findMsign(begin, end, &position);

        if (t == '+') return eval(begin + 1, position, xValue, dziedzina) + eval(position + 1, end - 1, xValue, dziedzina);
        else if (t == '-') return eval(begin + 1, position, xValue, dziedzina) - eval(position + 1, end - 1, xValue, dziedzina);
        else if (t == '*') return eval(begin + 1, position, xValue, dziedzina) * eval(position + 1, end - 1, xValue, dziedzina);
        else if (t == '/') 
        {
            double b =  eval(position + 1, end - 1, xValue, dziedzina);
            if (b == 0) 
            {
                *dziedzina = false;
                return 0;
            }
            return eval(begin + 1, position, xValue, dziedzina) / b;
        }
        else if (t == '^') 
        {
            double a = eval(begin + 1, position, xValue, dziedzina);
            double b = eval(position + 1, end - 1, xValue, dziedzina);
            if(a == 0 && b == 0)
            {
                *dziedzina = false;
                return 0;
            }
            return pow(a, b);
        }
        else return eval(begin + 1, end - 1, xValue, dziedzina);
    }

    if (*begin == 'x') return xValue;
    else if (*begin <= 'z' && *begin >= 'a') 
    {
        char* openBracket = searchF(begin, end);

        if (openBracket == NULL) segfault("Syntax error!");

        char function[10];

        for (int i = 0; i != openBracket - begin; i++) function[i] = *(begin + i);
        function[openBracket - begin] = '\0';

        if (strcmp(function, "sin") == 0) return sin(eval(openBracket + 1, end - 1, xValue, dziedzina));
        else if (strcmp(function, "cos") == 0) return cos(eval(openBracket + 1, end - 1, xValue, dziedzina));
        else if (strcmp(function, "tan") == 0 || strcmp(function, "tg") == 0) 
        {
            double a = sin(eval(openBracket + 1, end - 1, xValue, dziedzina));
            double b = cos(eval(openBracket + 1, end - 1, xValue, dziedzina));

            if (b == 0)
            {
                *dziedzina = false;
                return 0;
            }
            return a / b;
        }
        else if (strcmp(function, "cot") == 0 || strcmp(function, "ctg") == 0) 
        {
            double b = sin(eval(openBracket + 1, end - 1, xValue, dziedzina));
            double a = cos(eval(openBracket + 1, end - 1, xValue, dziedzina));

            if (b == 0) 
            {
                *dziedzina = false;
                return 0;
            }
            return a / b;
        }
        else if (strcmp(function, "log") == 0) 
        {
            double a = eval(openBracket + 1, end - 1, xValue, dziedzina);
            if (a < 0) 
            {
                *dziedzina = false;
                return 0;
            }
            return log(a);
        }
        else if (strcmp(function, "sqrt") == 0) 
        {
            double a = eval(openBracket + 1, end - 1, xValue, dziedzina);
            if (a < 0) 
            {
                *dziedzina = false;
                return 0;
            }
            return sqrt(a);
        }
        else 
        {
            char dest[33] = "";
            strcat(dest, "Nie istnieje funkcja ");
            strcat(dest, function);
            segfault(dest);
        }
    }

    char equation[1000];
    char* eptr;

    int i;
    for(i = 0; i < end - begin; i++) equation[i] = *(begin + i);
    equation[i] = '\0';
    
    return strtod(equation, &eptr);
}

char findMsign(char* begin, char* end, char** position)
{
    int openBrackets = 0;

    for (int i = 0; i < end - begin; i++)
    {
        if (*(begin + i) == '(') openBrackets++;
        else if (*(begin + i) == ')') openBrackets--;
        else if (*(begin + i) == '+' && openBrackets == 1) {*position = begin + i; return '+';}
        else if (*(begin + i) == '-' && openBrackets == 1) {if(i!=1){*position = begin + i; return '-';}}
        else if (*(begin + i) == '*' && openBrackets == 1) {*position = begin + i; return '*';}
        else if (*(begin + i) == '/' && openBrackets == 1) {*position = begin + i; return '/';}
        else if (*(begin + i) == '^' && openBrackets == 1) {*position = begin + i; return '^';}
    }

    return 'T';
}

char* searchF(char* begin, char* end)
{
    int i = 0;
    while (*(begin + i) != '(' && (begin + i) != end) i++;
    
    if ((begin + i) != end) return begin + i;
    return NULL;
}

void segfault(char info [])
{
    printf("Nieobs\210ugiwany wyj\245tek: %s\n", info);
    exit(0);
}