#include "includes.h"


#define _PATH_DEVPTS  "/dev/pts/"
#define _PATH_DEVPTMX "/dev/ptmx"


int
ptypair(int* amaster, int* aslave)
{
    int master, slave;
    char name[sizeof(_PATH_DEVPTS) + 20];

    master = open(_PATH_DEVPTMX, O_RDWR | O_NOCTTY);
    if (master < 0) {
#ifdef __DEBUG
        perror("ptypair: open master");
#endif
        return 0;
    }

    int unlock;
    unsigned int ptyno;

    if (
        ioctl(master, TIOCSPTLCK, &unlock) != 0 ||
        ioctl(master, TIOCGPTN, &ptyno) != 0    ||
        snprintf(name, sizeof(name), "%s%d", _PATH_DEVPTS, ptyno) < 0
    ) {
#ifdef __DEBUG
        perror("ptypair");
#endif
        close(master);
        return 0;
    }

    slave = open(name, O_RDWR);
    if (slave == -1) {
#ifdef __DEBUG
        perror("ptypair: open slave");
#endif
        close(master);
        return 0;
    }

    *amaster = master;
    *aslave = slave;

    return 1;
}
