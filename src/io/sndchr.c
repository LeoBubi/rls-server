#include "includes.h"


int
sndchr(int sockfd, char c)
{
    /* ----- send character ----- */

    if (write(sockfd, &c, 1) != 1) {
#ifdef __DEBUG
        perror("sndchr: write");
#endif
        return 0;
    }

    return 1;
}
