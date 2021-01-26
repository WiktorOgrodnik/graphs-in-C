#include "eval.h"
#define NUMBER 0

/* Definitions */

//Working on chars
static void return_char(int c, char **inp);
static int read_char(char **inp);

//Return number or func(x) or constant
static double read_value(char **inp, double xValue, bool* stop);

//Grammar terminals
static double expression(char **inp, double xValue, bool* stop);
static double ingredient(char **inp, double xValue, bool* stop);
static double factor(char **inp, double xValue, bool* stop);
static double exponent(char **inp, double xValue, bool* stop);

//Custom math functions
static double fractionalPart(double a);

double calc(const char* eqBegin, int eqLength, double calcPoint, bool* stop)
{
    char begin[eqLength];

    //Copy string to other array
    for (int i = 0; i <= eqLength; i++)
    {
        begin[i] = *(eqBegin + i);
    }

    int c;
    double result;
    char* inptr = begin;
    
    //If equation is not empty, calculate it
    if ((c = read_char(&inptr)) != EOF) 
    {
        return_char(c, &inptr);

        result = expression(&inptr, calcPoint, stop);
    }

    return result;
}

static void return_char(int c, char **inp)
{
    if (c != EOF && c != NUMBER) --*inp;
}

static int read_char(char **inp)
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

static double read_value(char **inp, double xValue, bool* stop)
{
    char error[100];
    int c;
    double n = 0.0, exp10 = 1.0;

    //Number - calculate part of the total
    while ((c = *(*inp)++) != '\0' && isdigit(c)) n = 10.0 * n + (c -'0');

    if (c == '.' || c == ',')
    {   
        //Number - calculate fractional part
        while ((c = *(*inp)++) != '\0' && isdigit(c)) 
        {
            n = 10.0 * n + (c - '0');
            exp10 *= 10.0;
        }
    }
    else if(c == 'x') //Variable
    {
        n = xValue; 
        c = *(*inp)++;
    }
    else if(c == 'e') //e constant
    {
        n = M_E; 
        c = *(*inp)++; 
    }
    else if(c == 'p')
    {
        c = *(*inp)++;
        if (c == 'i') //pi constant
        {
            n = M_PI;
            c = *(*inp)++;
        } 
        else if (c == 'h')
        {
            c = *(*inp)++;
            if (c == 'i') ///phi constant
            {
                n = 1.618033988749895;
                c = *(*inp)++;
            }
        }
    }
    /*
    To-do: Implement constants module
    */
    else if (isalpha(c)) // Function module
    {
        int it = 1;
        char function[10];
        function[0] = (char)c;

        while ((c = *(*inp)++) != '\0' && c != '(') //Read function name
        {
            function[it] = (char)c;
            it++;
        }
        function[it] = '\0';

        //Function switch - to-do: pointers to function
        if (strcmp(function, "sin") == 0) n = sin(expression(inp, xValue, stop)); 
        else if (strcmp(function, "cos") == 0) n = cos(expression(inp, xValue, stop));
        else if (strcmp(function, "tan") == 0 || strcmp(function, "tg") == 0) n = tan(expression(inp, xValue, stop));
        else if (strcmp(function, "cot") == 0 || strcmp(function, "ctg") == 0) n = 1 / tan(expression(inp, xValue, stop));
        else if (strcmp(function, "log") == 0 || strcmp(function, "ln") == 0) n = log(expression(inp, xValue, stop));
        else if (strcmp(function, "sqrt") == 0) n = sqrt(expression(inp, xValue, stop));
        else if (strcmp(function, "log10") == 0) n = log10(expression(inp, xValue, stop));
        else if (strcmp(function, "log2") == 0) n = log2(expression(inp, xValue, stop));
        else if (strcmp(function, "abs") == 0) n = fabs(expression(inp, xValue, stop));
        else if (strcmp(function, "sinh") == 0) n = sinh(expression(inp, xValue, stop));
        else if (strcmp(function, "cosh") == 0) n = cosh(expression(inp, xValue, stop));
        else if (strcmp(function, "tanh") == 0 || strcmp(function, "tgh") == 0) n = tanh(expression(inp, xValue, stop));
        else if (strcmp(function, "ceil") == 0) n = ceil(expression(inp, xValue, stop));
        else if (strcmp(function, "floor") == 0) n = floor(expression(inp, xValue, stop));
        else if (strcmp(function, "acos") == 0 || strcmp(function, "arccos") == 0) n = acos(expression(inp, xValue, stop));
        else if (strcmp(function, "asin") == 0 || strcmp(function, "arcsin") == 0) n = asin(expression(inp, xValue, stop));
        else if (strcmp(function, "atan") == 0 || strcmp(function, "arctan") == 0 || strcmp(function, "arctg") == 0) n = atan(expression(inp, xValue, stop));
        else if (strcmp(function, "exp") == 0) n = exp(expression(inp, xValue, stop));
        else 
        {
            sprintf(error, "Error: Can not find: %s\n", function);
	        error_dialog(error);

            *stop = true;

            return nan("OUT");
        }

        if ((c = read_char(inp)) != ')') 
        {
            sprintf(error, "Error: Incorrect parenthesis, expected ')'\n");
	        error_dialog(error);

            *stop = true;

            return nan("STOP");
        }
        c = NUMBER;
    }

    return_char(c == 0 ? EOF : c, inp);        
    return n / exp10;
}

static double expression(char **inp, double xValue, bool* stop) //Expression consist of ingredients and signs '+' and '-'
{
    int c;
    double res, x2;

    if ((c = read_char(inp)) != '-' && c != '+') return_char(c, inp);

    res = ingredient(inp, xValue, stop);

    if (c == '-') res = -res;

    while ((c = read_char(inp)) == '+' || c == '-') 
    {
        x2 = ingredient(inp, xValue, stop);
        res = (c == '+' ? res + x2 : res - x2);
    }
    return_char(c, inp);        
    return res;
}

static double ingredient(char **inp, double xValue, bool* stop) //ingredient consist of factors and sings '*' and '/'
{
    int c;
    double res, x2;

    res = factor(inp, xValue, stop);
    while ((c = read_char(inp)) == '*' || c == '/') 
    {
        x2 = factor(inp, xValue, stop);

        if (c == '*') res *= x2;
        else
        {
            if (x2) res /= x2;
            else return nan("OUT");
        }
    }

    return_char(c, inp);        
    return res;
}

static double factor(char **inp, double xValue, bool* stop) //factors consist of exponents and sign '^'
{
    int c;
    double res, x2;

    res = exponent(inp, xValue, stop);
    while ((c = read_char(inp)) == '^') 
    {
        x2 = factor(inp, xValue, stop);

        if (res || x2) res = pow(res, x2);
        else return nan("OUT");
    }

    return_char(c, inp);        
    return res;
}

static double exponent(char **inp, double xValue, bool* stop) //exponent can be number or function or variable or constant or parentheses '()', '{}', '||'
{
    int c;
    double res;
    char error[100];

    if ((c = read_char(inp)) == NUMBER) return read_value(inp, xValue, stop);
    else if (c == '(') 
    {
        res = expression(inp, xValue, stop);

        if ((c = read_char(inp)) == ')') return res;
        else 
        {
	        sprintf(error, "Error: expected ')'\n");
	        error_dialog(error);

            *stop = true;

            return nan("STOP");
        }
    }
    else if (c == '|')
    {
        res = fabs(expression(inp, xValue, stop));

        if ((c = read_char(inp)) == '|') return res;
        else 
        {
	        sprintf(error, "Error: expected ')'\n");
	        error_dialog(error);

            *stop = true;

            return nan("STOP");
        }
    }
    else if (c == '{')
    {
        res = fractionalPart(expression(inp, xValue, stop));

        if ((c = read_char(inp)) == '}') return res;
        else 
        {
	        sprintf(error, "Error: expected '}'\n");
	        error_dialog(error);

            *stop = true;

            return nan("STOP");
        }
    }
    else 
    {
        sprintf(error, "Error: expected number or '(', or '|', or '{'\n");
	    error_dialog(error);

        *stop = true;

        return nan("STOP");
    }
}

static double fractionalPart(double a)
{
    return a - floor(a);
}
