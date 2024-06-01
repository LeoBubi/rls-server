#include <includes.h>


sig_t
atosig(const char *s)
{
    return (sig_t)s[0];
}
