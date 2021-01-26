#include "convert.h"
#include <math.h>

static void reverse(char* str, int len);
static int int_to_string(int a, char str[], int d);

void double_to_char(double a, int afterpoint, char str[])
{ 
    int ipart = (int)a; 
    double dpart = a - (double)ipart; 
  
    int i = int_to_string(ipart, str, 0); 
  
    if (afterpoint != 0) { 

        str[i] = ','; 

        dpart = dpart * pow(10, afterpoint); 
  
        int_to_string((int)dpart, str + i + 1, afterpoint); 
    } 
}

static int int_to_string(int a, char str[], int d)
{
    if (a == 0) 
    {
        str[0] = '0';
        str[1] = '\0';
        return 1;
    }
    int i = 0; 
    while (a) { 
        str[i++] = (a % 10) + '0'; 
        a = a / 10; 
    }

    while (i < d) 
        str[i++] = '0';  
  
    reverse(str, i); 
    str[i] = '\0'; 

    return i;
}

static void reverse(char* str, int len) 
{ 
    int i = 0, j = len - 1, temp; 
    while (i < j) { 
        temp = str[i]; 
        str[i] = str[j]; 
        str[j] = temp; 
        i++; 
        j--; 
    } 
} 