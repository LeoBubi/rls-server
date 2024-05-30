#include "includes.h"


char* __gettxt(int sockfd);
char* __getsig(int sockfd);
char* __getctl(int sockfd);


char*
getmsg(int sockfd, char* type)
{
    /* ----- get message type ----- */

    char __type;

    if (read(sockfd, &__type, sizeof(__type)) == -1) {
#ifdef __DEBUG
        perror("getmsg: type: read");
#endif
        return NULL;
    }

    *type = __type;

    switch (__type) {
        case TXTMSG:
            return __gettxt(sockfd);
        case SIGMSG:
            return __getsig(sockfd);
        case CTLMSG:
            return __getctl(sockfd);
        default:
            return NULL;
    }
}


/* --------------------------------------------------- */


char*
__gettxt(int sockfd)
{
    /* ----- get message size ----- */

    size_t size;

    if (read(sockfd, &size, sizeof(size)) == -1) {
#ifdef __DEBUG
        perror("__gettxt: message size: read");
#endif
        return NULL;
    }

    /* ----- get message ----- */

    char* msg = (char*)malloc(size);
    if (msg == NULL) {
#ifdef __DEBUG
        perror("__gettxt: message: malloc");
#endif
        return NULL;
    }

    if (read(sockfd, msg, size) == -1) {
#ifdef __DEBUG
        perror("__gettxt: message: read");
#endif
        free(msg);
        return NULL;
    }

    return msg;
}


char* 
__getsig(int sockfd)
{
    /* ----- get signal code ----- */

    sig_t *sig = (sig_t*)malloc(1);
    if (sig == NULL) {
#ifdef __DEBUG
        perror("__getsig: malloc");
#endif
        return NULL;
    }

    if (read(sockfd, sig, 1) == -1) {
#ifdef __DEBUG
        perror("__getsig: read");
#endif
        free(sig);
        return NULL;
    }

    return (char*)sig;
}


char*
__getctl(int sockfd)
{
    /* ----- get command code ----- */

    ctl_t *ctl = (ctl_t*)malloc(1);
    if (ctl == NULL) {
#ifdef __DEBUG
        perror("__getctl: malloc");
#endif
        return NULL;
    }

    if (read(sockfd, ctl, 1) == -1) {
#ifdef __DEBUG
        perror("__getctl: read");
#endif
        free(ctl);
        return NULL;
    }

    return (char*)ctl;
}
