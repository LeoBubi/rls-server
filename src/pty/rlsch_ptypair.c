#include "includes.h"


int
rlsch_ptypair(int* amaster, int* aslave)
{
    int master, slave;
    char *name;

    master = rlsch_openpt(O_RDWR | O_NOCTTY);
    if (master < 0) {
#ifdef __DEBUG
        perror("rlsch_openpt");
#endif
        return 0;
    }

    if (rlsch_grantpt(master) < 0) {
#ifdef __DEBUG
        perror("rlsch_grantpt");
#endif
        close(master);
        return 0;
    }
    
    if (lsch_unlockpt(master) < 0) {
#ifdef __DEBUG
        perror("rlsch_unlockpt");
#endif
        close(master);
        return 0;
    }

    name = rlsch_ptsname(master);
    if (!name) {
#ifdef __DEBUG
        perror("rlsch_ptsname");
#endif
        close(master);
        return 0;
    }

    slave = open(name, O_RDWR);
    if (slave == -1) {
#ifdef __DEBUG
        perror("rlsch_ptypair: slave name: open");
#endif
        close(master);
        return 0;
    }

    *amaster = master;
    *aslave = slave;

    return 1;
}
