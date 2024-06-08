#include "includes.h"


static struct passwd __pwrec;
static char __pwrec_name[32+1]; // maximum username length + '\0'
static char __pwrec_passwd[73+1]; // password hash length + '\0'
static char __pwrec_gecos[256+1]; // maximum gecos length + '\0'
static char __pwrec_dir[PATH_MAX+1]; // maximum home path length + '\0' 
static char __pwrec_shell[PATH_MAX+1]; // maximum shell path length + '\0'


struct passwd*
getpwrec(const char* name)
{
    /* ----- initialize data structures ----- */

    memset(&__pwrec, 0, sizeof(struct passwd));
    memset(__pwrec_name, 0, sizeof(__pwrec_name));
    memset(__pwrec_passwd, 0, sizeof(__pwrec_passwd));
    memset(__pwrec_gecos, 0, sizeof(__pwrec_gecos));
    memset(__pwrec_dir, 0, sizeof(__pwrec_dir));
    memset(__pwrec_shell, 0, sizeof(__pwrec_shell));

    /* ----- open password file ----- */

    int pwfd = open("/etc/passwd", O_RDONLY);
    if (pwfd == -1) {
#ifdef __DEBUG
        perror("getpwrec: open");
#endif
        return NULL;
    }

    /* ----- read password file ----- */

    char *line;
    while ((line = rdline(pwfd)))
    {
        //printf("getpwrec: line: %s\n", line);

        char *tok = strtok(line, ":");

        if (strcmp(tok, name) == 0)
        {
            close(pwfd);

            memcpy(__pwrec_name, tok, strlen(tok)+1);
            __pwrec.pw_name = __pwrec_name;

            tok = strtok(NULL, ":");

            memcpy(__pwrec_passwd, tok, strlen(tok)+1);
            __pwrec.pw_passwd = __pwrec_passwd;

            tok = strtok(NULL, ":");

            __pwrec.pw_uid = atoi(tok);

            tok = strtok(NULL, ":");

            __pwrec.pw_gid = atoi(tok);

            tok = strtok(NULL, ":");

            memcpy(__pwrec_gecos, tok, strlen(tok)+1);
            __pwrec.pw_gecos = __pwrec_gecos;

            tok = strtok(NULL, ":");

            memcpy(__pwrec_dir, tok, strlen(tok)+1);
            __pwrec.pw_dir = __pwrec_dir;

            tok = strtok(NULL, ":");

            memcpy(__pwrec_shell, tok, strlen(tok)+1);
            __pwrec.pw_shell = __pwrec_shell;

            free(line);
            return &__pwrec;
        }

        free(line);
    }

    close(pwfd);
    return NULL;
}
