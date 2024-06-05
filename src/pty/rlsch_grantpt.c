#include "includes.h"


int
rlsch_grantpt(int fd)
{
    unsigned int ptyno;

    int ret = ioctl(fd, TIOCGPTN, &ptyno);
    if (ret != 0 && errno == ENOTTY)
        errno = EINVAL;
        
    return ret;
}
