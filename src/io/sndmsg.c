#include "includes.h"


int
sndmsg(int sockfd, const char *msg)
{
    /* ----- send message size ----- */

    size_t size = strlen(msg) + 1;  // include null terminator

    if (write(sockfd, &size, sizeof(size)) == -1) {
#ifdef __DEBUG
        perror("sndmsg: message size: write");
#endif
        return 0;
    }

    /* ----- send message ----- */

    if (write(sockfd, msg, size) == -1) {
#ifdef __DEBUG
        perror("sndmsg: message: write");
#endif
        return 0;
    }

    return 1;
}
