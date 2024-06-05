#include "includes.h"


/* Directory where we can find the slave pty nodes.  */
#define _PATH_DEVPTS "/dev/pts/"

/* Static buffer for 'ptsname'.  */
static char buffer[sizeof (_PATH_DEVPTS) + 20];


int __rlsch_ptsname_r(int fd, char *buf, size_t buflen);


char*
rlsch_ptsname(int fd)
{
    return __rlsch_ptsname_r(fd, buffer, sizeof(buffer)) != 0 ? NULL : buffer;
}


/* 
    Store at most BUFLEN characters of the pathname of the slave pseudo
    terminal associated with the master FD is open on in BUF.
    Return 0 on success, otherwise an error number.
*/

int
__rlsch_ptsname_r(int fd, char *buf, size_t buflen)
{
    int saved_errno = errno;
    unsigned int ptyno;

    if (ioctl(fd, TIOCGPTN, &ptyno) != 0)
        return errno;
    
    /*
        Buffer we use to print the number in.  
        For a maximum size forint of 8 bytes 
        we never need more than 20 digits.
    */
    char numbuf[21];
    const char *devpts = _PATH_DEVPTS;
    const size_t devptslen = strlen (_PATH_DEVPTS);
    char *p;

    numbuf[sizeof (numbuf) - 1] = '\0';
    p = _itoa_word(ptyno, &numbuf[sizeof (numbuf) - 1], 10, 0);

    if (buflen < devptslen + (&numbuf[sizeof (numbuf)] - p)) {
        errno = ERANGE;
        return errno;
    }

    memcpy (stpcpy(buf, devpts), p, &numbuf[sizeof (numbuf)] - p);

    errno = saved_errno;
    return 0;
}
