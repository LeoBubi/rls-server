#include "includes.h"


/* Directory where we can find the slave pty nodes.  */
#define _PATH_DEVPTS "/dev/pts/"

/* Static buffer for 'ptsname'.  */
static char buffer[sizeof (_PATH_DEVPTS) + 20];


char*
rlsch_ptsname(int fd)
{
    int saved_errno = errno;
    unsigned int ptyno;

    if (ioctl(fd, TIOCGPTN, &ptyno) != 0)
        return NULL;

    if (snprintf(buffer, sizeof(buffer), "%s%d", _PATH_DEVPTS, ptyno) < 0)
        return NULL;
    
    errno = saved_errno;
    return buffer;
}
