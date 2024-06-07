#include "includes.h"


char* __gettxt(int sockfd);
char* __getsig(int sockfd);
char* __getctl(int sockfd);
char* __getchr(int sockfd);


char*
getmsg(int sockfd, char* type)
{
    /* ----- get message type ----- */

    char __type;

    ssize_t rb = read(sockfd, &__type, sizeof(__type));
    if (rb == -1) {
#ifdef __DEBUG
        perror("getmsg: type: read");
#endif
        return NULL;
    }

    if (rb == 0) {
        *type = CLOSED;
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
        case CHRMSG:
            return __getchr(sockfd);
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

    rlssig_t *sig = (rlssig_t*)malloc(sizeof(rlssig_t));
    if (sig == NULL) {
#ifdef __DEBUG
        perror("__getsig: malloc");
#endif
        return NULL;
    }

    if (read(sockfd, sig, sizeof(rlssig_t)) == -1) {
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

    rlsctl_t *ctl = (rlsctl_t*)malloc(sizeof(rlsctl_t));
    if (ctl == NULL) {
#ifdef __DEBUG
        perror("__getctl: malloc");
#endif
        return NULL;
    }

    if (read(sockfd, ctl, sizeof(rlsctl_t)) == -1) {
#ifdef __DEBUG
        perror("__getctl: read");
#endif
        free(ctl);
        return NULL;
    }

    return (char*)ctl;
}


char*
__getchr(int sockfd)
{
    /* ----- get character ----- */

    char *chr = (char*)malloc(1);
    if (chr == NULL) {
#ifdef __DEBUG
        perror("__getchr: malloc");
#endif
        return NULL;
    }

    if (read(sockfd, chr, 1) == -1) {
#ifdef __DEBUG
        perror("__getchr: read");
#endif
        free(chr);
        return NULL;
    }

    return chr;
}
