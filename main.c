#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "eval.h"

#define LENGTH 1000

void scan_statement(char* statement);

int main()
{
    char str[LENGTH];

    scan_statement(str);

    printf("%lf", calc(str, strlen(str), 1.0));

    return 0;
}

void scan_statement(char* statement)
{
    char c = getchar();

    while(isspace(c)) c = getchar();

    while(true)
    {
        *statement++ = c;
        if(c == '\n') break;
        c = getchar();
    }

    *(statement - 1) = '\0';
}