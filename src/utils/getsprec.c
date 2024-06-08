#include "includes.h"


static struct spwd __sprec;
static char __sprec_name[32+1]; // maximum username length + '\0'
static char __sprec_hash[73+1]; // password hash length + '\0'


struct spwd*
getsprec(const char* name)
{
    /* ----- initialize data structures ----- */

    memset(&__sprec, 0, sizeof(struct spwd));
    memset(__sprec_name, 0, sizeof(__sprec_name));
    memset(__sprec_hash, 0, sizeof(__sprec_hash));

    /* ----- open shadow file ----- */

    int spfd = open("/etc/shadow", O_RDONLY);
    if (spfd == -1) {
#ifdef __DEBUG
        perror("getsprec: open");
#endif
        return NULL;
    }

    /* ----- read shadow file ----- */

    char *line;
    while ((line = rdline(spfd)))
    {
        //printf("getsprec: line: %s\n", line);

        char *tok = strtok(line, ":");

        if (strcmp(tok, name) == 0)
        {
            close(spfd);

            memcpy(__sprec_name, tok, strlen(tok)+1);
            __sprec.sp_namp = __sprec_name;

            tok = strtok(NULL, ":");

            memcpy(__sprec_hash, tok, strlen(tok)+1);
            __sprec.sp_pwdp = __sprec_hash;

            tok = strtok(NULL, ":");

            __sprec.sp_lstchg = atol(tok);

            tok = strtok(NULL, ":");

            __sprec.sp_min = atol(tok);

            tok = strtok(NULL, ":");

            __sprec.sp_max = atol(tok);

            tok = strtok(NULL, ":");

            __sprec.sp_warn = atol(tok);

            free(line);
            return &__sprec;
        }

        free(line);
    }

    close(spfd);
    return NULL;
}
