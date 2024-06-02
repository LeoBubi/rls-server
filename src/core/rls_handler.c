#include "includes.h"


extern int client_socket;   // client socket file descriptor

int pass_max_attempts;      // maximum number of password attempts

#define killshell() { \
    close(toshell[1]); \
    close(tohandler[0]); \
    kill(shell_pid, SIGKILL); \
}

// to be called at exit
void client_disconnected(void) {
    printf("Client disconnected.\n");
    return;
}


void handler_shutdown(int signo) {
    if (signo){;} // suppress warning
    sndack(client_socket, 50);
    close(client_socket);
    _exit(EXIT_SUCCESS);
}


void
rls_handler(void)
{
    char type;  // message type

    /* ----- register atexit functions ----- */

    if (atexit(client_disconnected) != 0) {
        sndack(client_socket, 50);
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    /* ----- set signal handlers ----- */

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler_shutdown;
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

    /* ----- acquire user info ----- */

    seteuid(0);     // gain root privileges

    struct spwd *sp = getspnam(username);

    seteuid(getuid());  // drop root privileges

    free(username);

    if (sp == NULL) {   // user does not exist
        sndack(client_socket, 40);
        close(client_socket);
        exit(EXIT_SUCCESS);
    }

    struct passwd *pw = getpwnam(sp->sp_namp);
    if (pw == NULL) {
        sndack(client_socket, 50);
        close(client_socket);
        exit(EXIT_FAILURE);
    } 

    if (pw->pw_shell == NULL || pw->pw_shell[0] == '\0') {  // no shell assigned
        sndack(client_socket, 40);
        close(client_socket);
        exit(EXIT_SUCCESS);
    }

    char *hashpass = sp->sp_pwdp;   // save password hash
    char *shell = pw->pw_shell;     // save shell
    uid_t uid = pw->pw_uid;         // save uid
    char *home = pw->pw_dir;        // save home directory

    sndack(client_socket, 20);  // username ok

    /* ----- receive password ----- */

    int attempts = 0;
    while (1)
    {
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

        if (++attempts >= pass_max_attempts) {
            sndack(client_socket, 41);  // too many attempts
            close(client_socket);
            exit(EXIT_SUCCESS); 
        }
        sndack(client_socket, 40);  // password incorrect
    }

    /* ----- become the new user ----- */

    setresuid(-1, 0, -1);   // gain root privileges

    // set environment variables
    clearenv();
    setenv("HOME", home, 1);
    setenv("USER", pw->pw_name, 1);
    setenv("LOGNAME", pw->pw_name, 1);
    setenv("SHELL", shell, 1);
    setenv("PATH", "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin", 1);
    setenv("LANG", "en_US.UTF-8", 1);
    setenv("TERM", "xterm-256color", 1);
    
    // go to user home directory
    if (chdir(home) == -1) {
#ifdef __DEBUG
        perror("chdir");
#endif
        sndack(client_socket, 50);
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // set user id
    setresuid(uid, uid, uid);    // drop root privileges and become the new user

    /* ----- open terminal session ----- */

    int toshell[2];     // handler to shell
    if (pipe(toshell) == -1) {
        sndack(client_socket, 50);
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    int tohandler[2];   // shell to handler
    if (pipe(tohandler) == -1) {
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
        close(tohandler[0]);
        close(tohandler[1]);
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
        close(tohandler[0]);

        dup2(toshell[0], STDIN_FILENO);
        dup2(tohandler[1], STDOUT_FILENO);
        dup2(tohandler[1], STDERR_FILENO);

        close(toshell[0]);
        close(tohandler[1]);

        sndack(client_socket, 20);  // terminal session ready to start

        /* ----- execute shell ----- */

        execl(shell, shell, "-i", NULL);
        exit(EXIT_FAILURE);
    }

    /* HANDLER */

    close(toshell[0]);
    close(tohandler[1]);

    fd_set __readfds;
    FD_ZERO(&__readfds);
    FD_SET(client_socket, &__readfds);
    FD_SET(tohandler[0], &__readfds);

    while (1)
    {
        /* ----- wait for client message or shell output ----- */

        fd_set readfds = __readfds;
        if (select((client_socket > tohandler[0] ? client_socket : tohandler[0]) + 1, &readfds, NULL, NULL, NULL) == -1) {
            sndack(client_socket, 50);
            killshell()
            exit(EXIT_FAILURE);
        }

        /* ----- client message ----- */

        if (FD_ISSET(client_socket, &readfds))
        {
            char *msg = getmsg(client_socket, &type);
            if (msg == NULL) {
                sndack(client_socket, 50);
                close(client_socket);
                killshell()
                exit(EXIT_FAILURE);
            }

            switch (type)
            {
                case TXTMSG:
                    if (write(toshell[1], msg, strlen(msg)+1) == -1) {
                        sndack(client_socket, 50);
                        free(msg);
                        close(client_socket);
                        killshell()
                        exit(EXIT_FAILURE);
                    }
                    sndack(client_socket, 20);
                    break;

                case SIGMSG:
                    if (kill(shell_pid, *(sig_t*)msg) == -1) {
                        sndack(client_socket, 50);
                        free(msg);
                        close(client_socket);
                        killshell()
                        exit(EXIT_FAILURE);
                    }
                    sndack(client_socket, 20);
                    break;
                
                case CTLMSG:
                    switch (*(ctl_t*)msg)
                    {
                        case CTLQUIT:
                            sndack(client_socket, 20);
                            write(toshell[1], "exit\n", sizeof("exit\n"));
                            free(msg);
                            break;
                        
                        default:
                            sndack(client_socket, 40);
                    }
                    break;

                default:
                    sndack(client_socket, 40);
            }

            free(msg);
        }

        /* ----- shell output ----- */

        else if (FD_ISSET(tohandler[0], &readfds))
        {
            char buf[BUFSIZ];
            memset(buf, '\0', BUFSIZ);
            ssize_t rb = read(tohandler[0], buf, BUFSIZ-1);
            if (rb == -1) {
                sndack(client_socket, 50);
                close(client_socket);
                killshell()
                exit(EXIT_FAILURE);
            }

            if (rb == 0) {  // shell closed
                sndack(client_socket, 50);
                close(client_socket);
                killshell()
                exit(EXIT_SUCCESS);
            }

            if (!sndmsg(client_socket, buf)) {
                close(client_socket);
                killshell()
                exit(EXIT_FAILURE);
            }
        }
    }
}
