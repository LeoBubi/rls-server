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
    
    // check message type
    if (type != TXTMSG) {
        sndack(client_socket, 40);
        free(username);
        close(client_socket);
        exit(EXIT_SUCCESS);
    }
    // root user not allowed
    if (strcmp(username, "root") == 0) {
        sndack(client_socket, 40);
        free(username);
        close(client_socket);
        exit(EXIT_SUCCESS);
    }

    // get user shadow record

    seteuid(0);     // gain root privileges

    struct spwd *sp = getspnam(username);

    seteuid(getuid());  // drop root privileges

    free(username);

    if (sp == NULL) {   // user does not exist
        sndack(client_socket, 40);
        close(client_socket);
        exit(EXIT_SUCCESS);
    }

    char *hashpass = sp->sp_pwdp;   // save hash password

    // get user passwd record

    struct passwd *pw = getpwnam(sp->sp_namp);
    if (pw == NULL) {
        sndack(client_socket, 50);
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // check if user is assigned a shell
    if (pw->pw_shell == NULL || pw->pw_shell[0] == '\0') {
        sndack(client_socket, 40);
        close(client_socket);
        exit(EXIT_SUCCESS);
    }

    char *shell = pw->pw_shell;     // save shell

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

        if (type == TXTMSG && strcmp(crypt(password, hashpass), hashpass) == 0) {
            free(password);
            break;  // password correct
        }

        free(password);
        sndack(client_socket, 40);  // password incorrect
    }

    /* ----- change uids ----- */

    struct passwd *pw = getpwnam(sp->sp_namp);
    if (pw == NULL) {
        sndack(client_socket, 50);
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    setresuid(pw->pw_uid, pw->pw_uid, pw->pw_uid);

    /* ----- open terminal session ----- */

    int toshell[2];
    if (pipe(toshell) == -1) {
        sndack(client_socket, 50);
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    int fromshell[2];
    if (pipe(fromshell) == -1) {
        sndack(client_socket, 50);
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    pid_t shell_pid = fork();
    if (shell_pid == -1) {
        sndack(client_socket, 50);
        close(client_socket);
        close(toshell[0]);
        close(toshell[1]);
        close(fromshell[0]);
        close(fromshell[1]);
        exit(EXIT_FAILURE);
    }

    if (shell_pid == 0) /* CHILD */
    {
        /* ----- set signal handlers ----- */

        struct sigaction sa;
        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = SIG_IGN;
        sigaction(SIGUSR1, &sa, NULL);

        /* ----- setup file descriptors ----- */

        close(toshell[1]);
        close(fromshell[0]);

        dup2(toshell[0], STDIN_FILENO);
        dup2(fromshell[1], STDOUT_FILENO);
        dup2(fromshell[1], STDERR_FILENO);

        close(toshell[0]);
        close(fromshell[1]);

        sndack(client_socket, 20);  // terminal session ready to start

        /* ----- execute shell ----- */

        execl(shell, shell, NULL);
        exit(EXIT_FAILURE);
    }

    /* HANDLER */

    
}
