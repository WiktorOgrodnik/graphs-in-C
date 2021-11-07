#include "eval.h"

// For error messages
static const char* exprtag_to_string (enum ExprTag tag);

static const char* exprtag_to_string (enum ExprTag tag) {

    switch (tag) {
        case CONST_EXPR: 
            return "CONST_EXPR";
            break;
        case VAR_EXPR: 
            return "VAR_EXPR";
            break;
        case BINOP_EXPR: 
            return "BINOP_EXPR";
            break;
        case FUN_EXPR: 
            return "FUN_EXPR";
            break;
        case ERROR_EXPR:
            return "ERROR_EXPR";
            break;
        default:
            return "UNDEFINED_EXPR";
            break;
    }
}

// Evaluate expression in point xValue
double eval (Expr* expression, double xValue, int* error, char* message) {

    double l, r, d; // left, right and result calucalted expressions

    switch (expression->tag) {
        case CONST_EXPR: 
            return expression->val1.data;
            break;
        case VAR_EXPR: 
            return xValue;
            break;
        case BINOP_EXPR:
            l = eval (expression->val2.left, xValue, error, message); // eval left part
            r = eval (expression->val3.right, xValue, error, message); // eval right part

            // errors (calcualte mode only)
            if (isnan (l) || isnan (r)) *error = 2;
            else if (expression->val1.binop == mdiv && r == 0) *error = 3;
            else if (expression->val1.binop == pow && l == 0 && r == 0) *error = 4;

            d = expression->val1.binop (l, r); // eval binary oparation

            if (isnan (d)) *error = 2;

            if (*error == 0) return d;
            return nan ("error");

            break;
        case FUN_EXPR: 
            d = expression->val1.function (eval (expression->val2.left, xValue, error, message));

            // errors (calculate mode only)
            if (isnan (d)) *error = 2;

            return d;
            break;
        default: 
            *error = 1;
            sprintf (message, "%s", exprtag_to_string (expression->tag));
            return nan("error");
    }
}