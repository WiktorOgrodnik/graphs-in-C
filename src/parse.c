#include "parse.h"
#define NUMBER 0

/* Definitions */

//Working on chars
static void return_char (int c, char** inp);
static int read_char (char** inp);

//Return number or func(x) or constant
static Expr* read_value (char** inp, int* error, char* message);

//Grammar not terminals
static Expr* expression (char** inp, int* error, char* message);
static Expr* ingredient (char** inp, int* error, char* message);
static Expr* factor (char** inp, int* error, char* message);
static Expr* exponent (char** inp, int* error, char* message);

static Expr* runtime_error (int* error, char* message);

char* nameOfFunctions[NUMBER_OF_FUNCTIONS] = {"sin", "cos", "tan", "tg", "cot", "ctg", "log", "ln", "sqrt", "log10", "log2", "abs", "sinh", "cosh", "tanh", "tgh", "ceil", "floor", "acos", "arccos", "asin", "arcsin", "atan", "arctan", "arctg", "exp"};
double (*functions[NUMBER_OF_FUNCTIONS])(double x) = {sin, cos, tan, tan, cot, cot, log, log, sqrt, log10, log2, fabs, sinh, cosh, tanh, tanh, ceil, floor, acos, acos, asin, asin, atan, atan, atan, exp};

char* nameOfConstants[NUMBER_OF_CONSTANTS] = {"e", "pi", "phi"};
double constants[NUMBER_OF_CONSTANTS] = {M_E, M_PI, 1.618033988749895};

//Print parse
static const char* fun_to_string (double (*fun)(double));
static const char* binop_to_string (double (*fun)(double, double));

static const char* fun_to_string (double (*fun)(double)) {
    for (int i = 0; i < NUMBER_OF_FUNCTIONS; i++) {
        if (fun == functions[i]) return nameOfFunctions[i];
    }

    return "null";
}

static const char* binop_to_string (double (*fun)(double, double)) {
    if (fun == pow) return "pow";
    if (fun == add) return "add";
    if (fun == sub) return "sub";
    if (fun == mult) return "mult";
    if (fun == mdiv) return "div";

    return "null";
}

char* to_string (Expr* expression) {

    char* napis = (char*) malloc (1000 * sizeof (char));

    switch (expression->tag) {
        case CONST_EXPR: sprintf (napis, "(const-expr %f)", expression->val1.data);
                    break;
        case VAR_EXPR: sprintf (napis, "(var-expr)");
                    break;
        case FUN_EXPR: sprintf (napis, "(fun-expr %s %s)", fun_to_string (expression->val1.function), to_string (expression->val2.left));
                    break;
        case BINOP_EXPR: sprintf (napis, "(binop-expr %s %s %s)", binop_to_string (expression->val1.binop), to_string (expression->val2.left), to_string (expression->val3.right));
                    break;
        case ERROR_EXPR: sprintf (napis, "(error-expr %d %s)", expression->val1.error, expression->val2.message);
                    break;
        default: sprintf (napis, "Something went wrog!"); break;
    }

    return napis;
}

static Expr* runtime_error (int* error, char* message) {

    *error = 4;
    return NULL;
}

Expr* parse (const char* eqBegin, int* error, char* message) {

    int eqLength = strlen (eqBegin);
    char begin[eqLength];

    // Copy string to other array
    for (int i = 0; i <= eqLength; i++)
        begin[i] = *(eqBegin + i);

    int c;
    Expr* result = NULL;
    char* inptr = begin;
    
    // If equation is not empty, calculate it
    if ((c = read_char (&inptr)) != EOF) {
        return_char (c, &inptr);

        result = expression (&inptr, error, message);
    }

    if (result == NULL) *error = 5;
    return result;
}

static void return_char (int c, char** inp) {
    if (c != EOF && c != NUMBER) --*inp;
}

static int read_char (char** inp) {
    int c;
    
    //EOF when '\0'
    if (**inp == '\0') return EOF;

    //Skip all spaces
    while ((c = *(*inp)++) != '\0' && isspace (c));

    //When char is allowed return 0
    if (isdigit (c) || c == '.' || c == ',' || isalpha (c)) {
        return_char (c, inp);
        return NUMBER;
    }
    return c == 0 ? EOF : c; //If 0 return EOF
}

static Expr* read_value (char** inp, int* error, char* message) {
    
    int c;
    double n = 0.0, exp10 = 1.0;
    bool isNum = false;
    bool isFunction = false;
    Expr* ex;

    if ((ex = (Expr*) malloc (sizeof (Expr))) == NULL)
        return runtime_error (error, message);

    if (isdigit (**inp)) {

        ex->tag = CONST_EXPR;

        //Number - calculate part of the total
        while ((c = *(*inp)++) != '\0' && isdigit (c)) {
            isNum = true;
            n = 10.0 * n + (c - '0');
        }

        if (c == '.' || c == ',') {   
            //Number - calculate fractional part
            while ((c = *(*inp)++) != '\0' && isdigit (c)) {
                n = 10.0 * n + (c - '0');
                exp10 *= 10.0;
            }
        }

        ex->val1.data = n / exp10;
    }

    if (c == 'x') { //Variable

        if (isNum) {
            
            Expr *ex1, *ex2;

            if ((ex1 = (Expr*) malloc (sizeof (Expr))) == NULL || (ex2 = (Expr*) malloc (sizeof (Expr))) == NULL)
                return runtime_error (error, message);

            ex1->tag = CONST_EXPR;
            ex2->tag = VAR_EXPR;

            ex1->val1.data = n / exp10;

            ex->tag = BINOP_EXPR;
            ex->val1.binop = mult;
            ex->val2.left = ex1;
            ex->val3.right = ex2;
        }
        else ex->tag = VAR_EXPR;

        c = *(*inp)++;
        c = *(*inp)++;
    }

    else if (isalpha (c)) { // Function module

        int it = 0;
        char function [10];

        while ((c = *(*inp)++) != '\0' && c != '(' && !isspace(c) && (isalpha (c) || isdigit (c))) //Read function or constant name
            function [it++] = (char)c;

        function [it] = '\0'; // Set null character to the end of the string

        while (isspace (**inp) || **inp == '(') // Skip spaces before openinig bracket
           c = *(*inp)++;

        bool found = false;
        bool constant = false;

        for (int i = 0; i < NUMBER_OF_FUNCTIONS && !found; i++) { //Find a specific function

            if (strcmp (function, nameOfFunctions [i]) == 0) {
                
                found = true;
                isFunction = true;

                if (isNum) {

                    Expr *ex1, *ex2;

                    if ((ex1 = (Expr*) malloc (sizeof (Expr))) == NULL || (ex2 = (Expr*) malloc (sizeof (Expr))) == NULL)
                        return runtime_error (error, message);

                    ex1->tag = CONST_EXPR;
                    ex1->val1.data = n / exp10;

                    ex2->tag = FUN_EXPR;
                    ex2->val1.function = functions [i];
                    ex2->val2.left = expression (inp, error, message);

                    ex->tag = BINOP_EXPR;
                    ex->val1.binop = mult;
                    ex->val2.left = ex1;
                    ex->val3.right = ex2;
                }
                else {
                    ex->tag = FUN_EXPR;
                    ex->val1.function = functions [i];
                    ex->val2.left = expression (inp, error, message);
                }
            }
        }

        if (!found) {

            for (int i = 0; i < NUMBER_OF_CONSTANTS && !found; i++) { //Find a specyfic constant
                
                if (strcmp (function, nameOfConstants [i]) == 0) {

                    found = true;
                    constant = true;

                    if (isNum) {
                        ex->tag = CONST_EXPR;
                        ex->val1.data = (n / exp10) * constants [i];
                    }
                    else { 
                        ex->tag = CONST_EXPR;
                        ex->val1.data = constants [i];
                    }

                    return_char (c, inp);
                }
            }

            if (!found) {
                
                *error = 1;
                sprintf (message, function);

                ex->tag = ERROR_EXPR;
                ex->val1.error = 1;

                ex->val2.message = (char*) malloc ((strlen (function) + 1) * sizeof (char));
                sprintf (ex->val2.message, function);
                return ex;
            }
        }

        if (isFunction) {
            while ((c = *(*inp)++) != ')');
        }

        return_char (c, inp);

        if (!constant && (c = read_char (inp)) != ')') {

            *error = 3;
            sprintf (message, ")");

            ex->tag = ERROR_EXPR;
            ex->val1.error = 3;

            ex->val2.message = (char*) malloc (2 * sizeof (char));
            sprintf (ex->val2.message, ")");

            return ex;
        }

        c = NUMBER;
    }

    return_char (c == 0 ? EOF : c, inp);        
    return ex;
}

static Expr* expression (char** inp, int* error, char* message) { //Expression consist of ingredients and signs '+' and '-'

    int c;
    Expr *res;

    if ((c = read_char (inp)) != '-' && c != '+') return_char (c, inp); //detect sign of ingredient

    res = ingredient (inp, error, message); //find next ingredient

    if (c == '-') { //if it was negative ingredient multiply by -1 
        
        Expr* ex1;
        Expr* ex2 = res;

        if ((ex1 = (Expr*) malloc (sizeof (Expr))) == NULL || (res = (Expr*) malloc (sizeof (Expr))) == NULL)
            return runtime_error (error, message);

        ex1->tag = CONST_EXPR;
        ex1->val1.data = -1;

        res->tag = BINOP_EXPR;
        res->val1.binop = mult;
        res->val2.left = ex1;
        res->val3.right = ex2;
    }     

    while ((c = read_char (inp)) == '+' || c == '-') { //if the next character is '+' or '-' calculate the expression
    
        Expr* ex1 = res;
        Expr* ex2 = ingredient (inp, error, message); //find next ingredient

        if ((res = (Expr*) malloc (sizeof (Expr))) == NULL)
            return runtime_error (error, message);

        res->tag = BINOP_EXPR;
        res->val2.left = ex1;
        res->val3.right = ex2;

        if (c == '+') res->val1.binop = add;
        else res->val1.binop = sub;
    }

    return_char (c, inp); //return last character (used for check if next is '+' or '-') 
    return res;
}

static Expr* ingredient (char** inp, int* error, char* message) { //Expression consist of ingredients and signs '+' and '-'

    int c;
    Expr *res;

    res = factor (inp, error, message); //find next ingredient

    while ((c = read_char (inp)) == '*' || c == '/') { //if the next character is '*' or '/' calculate the expression

        Expr *ex1, *ex2;

        ex1 = res;
        ex2 = factor (inp, error, message); //find next ingredient

        if (((res = (Expr*) malloc (sizeof (Expr))) == NULL))
            return runtime_error (error, message);

        res->tag = BINOP_EXPR;
        res->val2.left = ex1;
        res->val3.right = ex2;

        if (c == '*') res->val1.binop = mult;
        else res->val1.binop = mdiv;
    }

    return_char (c, inp); //return last character (used for check if next is '*' or '/')
    return res;
}

static Expr* factor (char** inp, int* error, char* message) { //factors consist of exponents and sign '^'

    int c;
    Expr *res;

    res = exponent (inp, error, message); //find next exponent

    while ((c = read_char (inp)) == '^') { //if the next character is '^' calculate the factor

        Expr* ex1 = res;
        Expr* ex2 = factor (inp, error, message); //find next factor (becouse the stange order of actions with ^ sign)

        if (((res = (Expr*) malloc (sizeof (Expr))) == NULL))
            return runtime_error (error, message);

        res->tag = BINOP_EXPR;
        res->val1.binop = pow;
        res->val2.left = ex1;
        res->val3.right = ex2;
    }

    return_char (c, inp); //return last character (used for check if next is '^')     
    return res;
}

static Expr* exponent (char** inp, int* error, char* message) { //exponent can be number or function or variable or constant or parentheses '()', '{}', '||', '[]'
    
    int c;
    Expr* res;

    if ((res = (Expr*) malloc (sizeof (Expr))) == NULL)
        return runtime_error (error, message);

    if ((c = read_char (inp)) == NUMBER) return read_value (inp, error, message); //if the next character is number, function, constant or variable read value
    else if (c == '(') { // normal parenthesis, calculate the value in parentheses
        res = expression (inp, error, message);

        if ((c = read_char (inp)) == ')') return res;
        else {

            *error = 3;
            sprintf (message, ")");

            res->tag = ERROR_EXPR;
            res->val1.error = 3;
            res->val2.message = (char*) malloc (sizeof (char) * 2);
            sprintf (res->val2.message, ")");

            return res;
        }
    }

    else if (c == '|') { // the absolute value, calculate the absolute value in parentheses

        res->tag = FUN_EXPR;
        res->val1.function = fabs;
        res->val2.left = expression (inp, error, message);

        if ((c = read_char (inp)) == '|') return res;
        else {

            *error = 3;
            sprintf (message, "|");

            res->tag = ERROR_EXPR;
            res->val1.error = 3;
            res->val2.message = (char*) malloc (sizeof (char) * 2);
            sprintf (res->val2.message, "|");

            return res;
        }
    }
    else if (c == '{') { // the fractional part, calculate the fractional part of value in parentheses

        res->tag = FUN_EXPR;
        res->val1.function = fractionalPart;
        res->val2.left = expression (inp, error, message);

        if ((c = read_char (inp)) == '}') return res;
        else {

            *error = 3;
            sprintf (message, "}");

            res->tag = ERROR_EXPR;
            res->val1.error = 3;
            res->val2.message = (char*) malloc (sizeof (char) * 2);
            sprintf (res->val2.message, "}");

            return res;
        }
    }
    else if (c == '[') { // floor, calculate floor value in parentheses

        res->tag = FUN_EXPR;
        res->val1.function = floor;
        res->val2.left = expression (inp, error, message);

        if ((c = read_char (inp)) == ']') return res;
        else {

            *error = 3;
            sprintf (message, "]");

            res->tag = ERROR_EXPR;
            res->val1.error = 3;
            res->val2.message = (char*) malloc (sizeof (char) * 2);
            sprintf (res->val2.message, "]");

            return res;
        }
    }
    else { //Undefined character
        res->tag = ERROR_EXPR;
        res->val1.error = 2;
        res->val2.message = (char*) malloc (sizeof (char) * 5);
        sprintf (res->val2.message, "%s", *inp);

        return res;
    }
}

//Fractional part
double fractionalPart (double a) {
    return a - floor (a);
}

//Cotangens
double cot (double a) {
    if (tan (a) == 0) return nan ("out");
    return 1 / tan (a);
}

double add (double a, double b) {
    return a + b;
}

double sub (double a, double b) {
    return a - b;
}

double mult (double a, double b) {
    return a * b;
}

double mdiv (double a, double b) {
    return a / b;
}
