#include "includes.h"


int
sndack(int sockfd, char code)
{
    if (write(sockfd, &code, sizeof(code)) != 1) {
#ifdef __DEBUG
        perror("sndack: write");
#endif
        return 0;
    }

    return 1;
}
