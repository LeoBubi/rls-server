#include "includes.h"


extern int client_socket;   // client socket file descriptor

int pass_max_attempts;      // maximum number of password attempts


void controlled_shutdown(int signo) {
    if (signo); // suppress warning
    sndack(client_socket, 50);
    close(client_socket);
    _exit(EXIT_SUCCESS);
}


void
rls_handler(void)
{
    char type;  // message type

    /* ----- set signal handlers ----- */

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = controlled_shutdown;
    sigaction(SIGUSR1, &sa, NULL);

    sndack(client_socket, 20);

    /* ----- receive username ----- */

    char *username = getmsg(client_socket, &type);
    if (username == NULL) {
        sndack(client_socket, 50);
        close(client_socket);
        exit(EXIT_FAILURE);
    }
    if (type != TXTMSG) {
        sndack(client_socket, 40);
        free(username);
        close(client_socket);
        exit(EXIT_SUCCESS);
    }
    if (strcmp(username, "root") == 0) {    // root user not allowed
        sndack(client_socket, 40);
        free(username);
        close(client_socket);
        exit(EXIT_SUCCESS);
    }

    /* ----- get user shadow record ----- */

    seteuid(0);     // gain root privileges

    struct spwd *user = getspnam(username);

    seteuid(getuid());  // drop root privileges

    if (user == NULL) {
        sndack(client_socket, 40);
        free(username);
        close(client_socket);
        exit(EXIT_SUCCESS);
    }

    free(username);
    sndack(client_socket, 20);  // username ok

    /* ----- receive password ----- */

    for (int attempts = 1;; attempts++)
    {
        if (attempts > pass_max_attempts) {
            sndack(client_socket, 41);
            close(client_socket);
            exit(EXIT_FAILURE);
        }

        char *password = getmsg(client_socket, &type);
        if (password == NULL) {
            sndack(client_socket, 50);
            close(client_socket);
            exit(EXIT_FAILURE);
        }

        // verify password
        if (type == TXTMSG && strcmp(crypt(password, user->sp_pwdp), user->sp_pwdp) == 0) {
            free(password);
            sndack(client_socket, 20);  // password ok
            break;
        }

        free(password);
        sndack(client_socket, 40);  // password incorrect
    }

    /* ----- change uids ----- */

    struct passwd *pw = getpwnam(user->sp_namp);
    if (pw == NULL) {
        sndack(client_socket, 50);
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    setresuid(pw->pw_uid, pw->pw_uid, pw->pw_uid);

    /* ----- open terminal session ----- */
}
