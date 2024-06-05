#include "includes.h"


int
rlsch_unlockpt(int fd)
{
    int unlock = 0;

    int ret = ioctl(fd, TIOCSPTLCK, &unlock);
    if (ret != 0 && errno == ENOTTY)
        errno = EINVAL;
    
    return ret;
}
