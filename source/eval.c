#include "eval.h"
#define NUMBER 0

/* Definitions */

//Working on chars
static void return_char(int c, char** inp);
static int read_char(char** inp);

//Return number or func(x) or constant
static double read_value(char** inp, double xValue, int* error, char* message);

//Grammar not terminals
static double expression(char** inp, double xValue, int* error, char* message);
static double ingredient(char** inp, double xValue, int* error, char* message);
static double factor(char** inp, double xValue, int* error, char* message);
static double exponent(char** inp, double xValue, int* error, char* message);

//Custom math functions
static double fractionalPart(double a);
static double cot(double a);

#define NUMBER_OF_FUNCTIONS 26
#define NUMBER_OF_CONSTANTS 3

static char* nameOfFunctions[NUMBER_OF_FUNCTIONS] = {"sin", "cos", "tan", "tg", "cot", "ctg", "log", "ln", "sqrt", "log10", "log2", "abs", "sinh", "cosh", "tanh", "tgh", "ceil", "floor", "acos", "arccos", "asin", "arcsin", "atan", "arctan", "arctg", "exp"};
static double (*functions[NUMBER_OF_FUNCTIONS])(double x) = {sin, cos, tan, tan, cot, cot, log, log, sqrt, log10, log2, fabs, sinh, cosh, tanh, tanh, ceil, floor, acos, acos, asin, asin, atan, atan, atan, exp};

static char* nameOfConstants[NUMBER_OF_CONSTANTS] = {"e", "pi", "phi"};
static double constants[NUMBER_OF_CONSTANTS] = {M_E, M_PI, 1.618033988749895};

double calc(const char* eqBegin, int eqLength, double calcPoint, int* error, char* message)
{
    char begin[eqLength];

    //Copy string to other array
    for (int i = 0; i <= eqLength; i++)
        begin[i] = *(eqBegin + i);

    int c;
    double result;
    char* inptr = begin;
    
    //If equation is not empty, calculate it
    if ((c = read_char(&inptr)) != EOF) 
    {
        return_char(c, &inptr);

        result = expression(&inptr, calcPoint, error, message);
    }

    return result;
}

static void return_char(int c, char** inp)
{
    if (c != EOF && c != NUMBER) --*inp;
}

static int read_char(char** inp)
{
    int c;
    
    //EOF when '\0'
    if (**inp == '\0') return EOF;

    //Skip all spaces
    while ((c = *(*inp)++) != '\0' && isspace(c));

    //When char is allowed return 0
    if (isdigit(c) || c == '.' || c == ',' || isalpha(c)) 
    {
        return_char(c, inp);
        return NUMBER;
    }
    return c == 0 ? EOF : c; //If 0 return EOF
}

static double read_value(char** inp, double xValue, int* error, char* message)
{
    int c;
    double n = 0.0, exp10 = 1.0;
    bool isNum = false;

    //Number - calculate part of the total
    while ((c = *(*inp)++) != '\0' && isdigit(c)) 
    {
        isNum = true;
        n = 10.0 * n + (c -'0');
    }

    if (c == '.' || c == ',')
    {   
        //Number - calculate fractional part
        while ((c = *(*inp)++) != '\0' && isdigit(c)) 
        {
            n = 10.0 * n + (c - '0');
            exp10 *= 10.0;
        }
    }
    if(c == 'x') //Variable
    {
        if (isNum) n *= xValue;
        else n = xValue; 
        c = *(*inp)++;
    }
    else if (isalpha(c)) // Function module
    {
        int it = 1;
        char function[10];
        function[0] = (char)c;

        while ((c = *(*inp)++) != '\0' && c != '(' && !isspace(c) && (isalpha(c) || isdigit(c))) //Read function or constant name
            function[it++] = (char)c;
    
        function[it] = '\0';
        
        bool found = false;
        bool constant = false;
        for (int i = 0; i < NUMBER_OF_FUNCTIONS && !found; i++) //Find a specific function
        {
            if (strcmp(function, nameOfFunctions[i]) == 0) 
            {
                found = true;
                if (isNum) n *= functions[i](expression(inp, xValue, error, message));
                else n = functions[i](expression(inp, xValue, error, message));
            }
        }

        if (!found)
        {
            for(int i = 0; i < NUMBER_OF_CONSTANTS && !found; i++) //Find a specyfic constant
            {
                if(strcmp(function, nameOfConstants[i]) == 0)
                {
                    found = true;
                    constant = true;
                    if(isNum) n *= constants[i];
                    else n = constants[i];

                    return_char(c, inp);
                }
            }

            if (!found)
            {
                *error = 1;
                message = function;

                return nan("OUT");
            }
        }

        if (!constant && (c = read_char(inp)) != ')') 
        {
            *error = 3;
            sprintf(message, ")");

            return nan("STOP");
        }

        c = NUMBER;
    }

    return_char(c == 0 ? EOF : c, inp);        
    return n / exp10;
}

static double expression(char** inp, double xValue, int* error, char* message) //Expression consist of ingredients and signs '+' and '-'
{
    int c;
    double res, x2;

    if ((c = read_char(inp)) != '-' && c != '+') return_char(c, inp); //detect sign of ingredient

    res = ingredient(inp, xValue, error, message); //find next ingredient

    if (c == '-') res *= -1; //if it was negative ingredient multiply by -1 

    while ((c = read_char(inp)) == '+' || c == '-') //if the next character is '+' or '-' calculate the expression
    {
        x2 = ingredient(inp, xValue, error, message); //find next ingredient
        res = (c == '+' ? res + x2 : res - x2);
    }

    return_char(c, inp); //return last character (used for check if next is '+' or '-')        
    return res;
}

static double ingredient(char** inp, double xValue, int* error, char* message) //ingredient consist of factors and sings '*' and '/'
{
    int c;
    double res, x2;

    res = factor(inp, xValue, error, message); //Find next factor
    while ((c = read_char(inp)) == '*' || c == '/') //if the next character is '*' or '/' calculate the ingredient
    {
        x2 = factor(inp, xValue, error, message); //Find next factor

        if (c == '*') res *= x2;
        else
        {
            if (x2) res /= x2;
            else return nan("OUT");
        }
    }

    return_char(c, inp); //return last character (used for check if next is '*' or '/')  
    return res;
}

static double factor(char** inp, double xValue, int* error, char* message) //factors consist of exponents and sign '^'
{
    int c;
    double res, x2;

    res = exponent(inp, xValue, error, message); //find next exponent
    while ((c = read_char(inp)) == '^') //if the next character is '^' calculate the factor
    {
        x2 = factor(inp, xValue, error, message); //find next factor (becouse the stange order of actions with ^ sign)

        if (res || x2) res = pow(res, x2);
        else return nan("OUT");
    }

    return_char(c, inp); //return last character (used for check if next is '^')       
    return res;
}

static double exponent(char** inp, double xValue, int* error, char* message) //exponent can be number or function or variable or constant or parentheses '()', '{}', '||', '[]'
{
    int c;
    double res;

    if ((c = read_char(inp)) == NUMBER) return read_value(inp, xValue, error, message); //if the next character is number, function, constant or variable read value
    else if (c == '(') // normal parenthesis, calculate the value in parentheses
    {
        res = expression(inp, xValue, error, message);

        if ((c = read_char(inp)) == ')') return res;
        else 
        {
            *error = 3;
            sprintf(message, ")");

            return nan("STOP");
        }
    }
    else if (c == '|') // the absolute value, calculate the absolute value in parentheses
    {
        res = fabs(expression(inp, xValue, error, message));

        if ((c = read_char(inp)) == '|') return res;
        else 
        {
            *error = 3;
            sprintf(message, "|");

            return nan("STOP");
        }
    }
    else if (c == '{') // the fractional part, calculate the fractional part of value in parentheses
    {
        res = fractionalPart(expression(inp, xValue, error, message));

        if ((c = read_char(inp)) == '}') return res;
        else 
        {
            *error = 3;
            sprintf(message, "}");

            return nan("STOP");
        }
    }
    else if (c == '[') // floor, calculate floor value in parentheses
    {
        res = floor(expression(inp, xValue, error, message));

        if ((c = read_char(inp)) == ']') return res;
        else 
        {
            *error = 3;
            sprintf(message, "]");

            return nan("STOP");
        }
    }
    else //Undefined character
    {
        *error = 2;

        return nan("STOP");
    }
}

//Fractional part
static double fractionalPart(double a)
{
    return a - floor(a);
}

//Cotangens
static double cot(double a)
{
    if (tan(a) == 0) return nan("out");
    return 1 / tan(a);
}
