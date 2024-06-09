#include "includes.h"


int
isint(const char *s)
{
    if (s == NULL || *s == '\0')
        return 0;

    int i;
    for (i = 0; s[i] != '\0'; i++)
        if ((s[i] < '0' || s[i] > '9') && (i != 0 || s[i] != '-'))
            return 0;
    
    if (i > 10)     // 10 digits is the maximum for a 32-bit integer
        return 0;
    
    long int n = atol(s);
    if (n < INT_MIN || n > INT_MAX)
        return 0;

    return 1;
}