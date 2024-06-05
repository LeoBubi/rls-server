#include "includes.h"


int
rlsch_ptypair(int* amaster, int* aslave)
{
    int master, slave;
    char *name;

    master = rlsch_openpt(O_RDWR | O_NOCTTY);
    if (master < 0)
        return 0;
    
    if (rlsch_grantpt(master) < 0 || rlsch_unlockpt(master) < 0) {
        close(master);
        return 0;
    }

    name = rlsch_ptsname(master);
    if (!name) {
        close(master);
        return 0;
    }

    slave = open(name, O_RDWR);
    if (slave == -1) {
        close(master);
        return 0;
    }

    *amaster = master;
    *aslave = slave;

    return 1;
}
