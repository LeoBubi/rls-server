#include "includes.h"


int 
rlsch_openpt(int flags)
{
    return open("/dev/ptmx", flags);
}
