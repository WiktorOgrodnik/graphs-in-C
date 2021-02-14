#include "convert.h"

double char_to_double(const char str[])
{
    /**
     * @brief Convert strings from entryboxes to double
     * 
     * @return converted number
     */
    int c, i = 0;
    const char* inp = str;
    double n = 0.0, exp10 = 1.0;

    //Number - calculate part of the total
    while ((c = *(inp + i++)) != '\0' && isdigit(c)) n = 10.0 * n + (c - '0');

    if (c == '.' || c == ',')
    {   
        //Number - calculate fractional part
        while ((c = *(inp + i++)) != '\0' && isdigit(c)) 
        {
            n = 10.0 * n + (c - '0');
            exp10 *= 10.0;
        }
    }

    return n / exp10;
}