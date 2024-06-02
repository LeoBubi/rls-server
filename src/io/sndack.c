#include "includes.h"


int
sndack(int sockfd, ack_t ack)
{
    /* ----- send ack ----- */

    if (write(sockfd, &ack, sizeof(ack)) != 1) {
#ifdef __DEBUG
        perror("sndack: write");
#endif
        return 0;
    }

    return 1;
}
