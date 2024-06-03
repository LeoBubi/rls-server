#include <includes.h>


pid_t listener_pid;


void server_shutdown(int signo) {
    if (signo){;} // suppress warning
    write(STDOUT_FILENO, "Shutting down server...\n", sizeof("Shutting down server...\n"));
    kill(listener_pid, SIGUSR1);
    waitpid(listener_pid, NULL, 0);
    exit(EXIT_SUCCESS);
}

void abnormal_termination(int signo) {
    if (signo){;} // suppress warning
    waitpid(listener_pid, NULL, 0);
    exit(EXIT_FAILURE);
}


int
main(int argc, char const *argv[])
{
    /* ----- initial checks ----- */

    if (!rls_server_checks())
        exit(EXIT_FAILURE);
    
    /* ----- initialize rls server ----- */

    if (!rls_server_initialize(argc, argv)) {
        fprintf(stderr, "Initialization failure.\n");
        exit(EXIT_FAILURE);
    }

    /* ----- initialize signal handlers ----- */

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_IGN;

    sigaction(SIGINT,  &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    sigaction(SIGPIPE, &sa, NULL);

    /* ----- create listener process ----- */

    listener_pid = fork();
    if (listener_pid == -1) {
#ifdef __DEBUG
        perror("fork");
#else
        fprintf(stderr, "Server startup failure.\n");
#endif
        exit(EXIT_FAILURE);
    }

    if (listener_pid == 0)
        rls_listener(); // never returns

    /* ----- set signal handlers ----- */

    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGINT);
    sigaddset(&sa.sa_mask, SIGQUIT);
    sigaddset(&sa.sa_mask, SIGUSR2);

    sa.sa_handler = server_shutdown;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);

    sa.sa_handler = abnormal_termination;
    sigaction(SIGUSR2, &sa, NULL);

    /* ----- monitor user input ----- */

    while (1)
    {
        char *input = userinput(NULL);
        if (input == NULL)
            continue;
        
        if (strcmp(input, "quit") == 0) {
            free(input);
            kill(getpid(), SIGQUIT);
        }

        free(input);
        printf("Use 'quit' to shutdown the server.\n");
    }
}
