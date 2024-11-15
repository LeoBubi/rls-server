#include <includes.h>


int port;               // server port number
int maxconn;            // maximum number of connections
int pass_max_attempts;  // maximum number of password attempts
int connto;             // client communication delay limit
int wpdelay;            // delay between wrong password attempts

int server_socket;    // server socket file descriptor
int client_socket;    // client socket file descriptor

volatile sig_atomic_t connections = 0;  // number of active connections

#define destroy_zombies() {                 \
    while(waitpid(-1, NULL, WNOHANG) > 0)   \
        connections--;                      \
}

#define kill_handlers() {                           \
    seteuid(0);         /* gain root privileges */  \
    kill(0, SIGUSR1);                               \
    seteuid(getuid());  /* drop root privileges */  \
    while(wait(NULL) > 0){;}                        \
}


void server_shutdown(int signo) {
    if (signo){;} // suppress warning
    write(STDOUT_FILENO, "Shutting down server...\n", sizeof("Shutting down server...\n"));
    close(server_socket);
    if (fcntl(client_socket, F_GETFD) != -1) {  // close client socket if open
        sndack(client_socket, 50);
        close(client_socket);
    }
    kill_handlers();
    _exit(EXIT_SUCCESS);
}


int
main(int argc, char const *argv[])
{
    /* ----- initial checks ----- */

    if (!rlss_check())
        exit(EXIT_FAILURE);
    
    /* ----- initialize rls server ----- */

    if (!rlss_init(argc, argv)) {
        fprintf(stderr, "Initialization failure.\n");
        exit(EXIT_FAILURE);
    }

    /* ----- set signal handlers ----- */

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_IGN;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    sigaction(SIGPIPE, &sa, NULL);

    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGINT);
    sigaddset(&sa.sa_mask, SIGQUIT);

    sa.sa_handler = server_shutdown;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);

    /* ----- start listening for client connections ------ */

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // open socket

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
#ifdef __DEBUG
        perror("rls-server: socket");
#else
        fprintf(stderr, "Unable to start listening for client connections.\n");
#endif
        exit(EXIT_FAILURE);
    }

    // set socket options

    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
#ifdef __DEBUG
        perror("rls-server: setsockopt for reusing address");
#else
        fprintf(stderr, "Unable to start listening for client connections.\n");
#endif
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // bind socket

    if (port < 1024) seteuid(0);    // gain root privileges

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
#ifdef __DEBUG
        perror("rls-server: bind");
#else
        fprintf(stderr, "Unable to start listening for client connections.\n");
#endif
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (port < 1024) seteuid(getuid());     // drop root privileges

    // make socket passive

    if (listen(server_socket, (maxconn < SOMAXCONN ? maxconn : SOMAXCONN)) == -1) {
#ifdef __DEBUG
        perror("rls-server: listen");
#else
        fprintf(stderr, "Unable to start listening for client connections.\n");
#endif
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Listening for client connections on port %d...\n", port);

    /* ----- wait for user input or client connection ----- */

    fd_set __readfds;
    FD_ZERO(&__readfds);
    FD_SET(server_socket, &__readfds);
    FD_SET(STDIN_FILENO, &__readfds);

    // client socket r/w timeout
    struct timeval timeout; 
    timeout.tv_sec = (time_t)connto;
    timeout.tv_usec = 0;

    while (1)
    {
        destroy_zombies();  // destroy defunct children

        fd_set readfds = __readfds;
        if (select(server_socket +1, &readfds, NULL, NULL, NULL) == -1) {
            if (errno == EINTR)
                continue;
#ifdef __DEBUG
            perror("rls-server: select");
#else
            fprintf(stderr, "An internal error occurred");
#endif
            close(server_socket);
            kill_handlers();
            exit(EXIT_FAILURE);
        }

        /* ----- client connection ----- */

        if (FD_ISSET(server_socket, &readfds))
        {
            struct sockaddr_in client_addr;
            socklen_t client_addr_len = sizeof(client_addr);

            client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
            if (client_socket == -1) {
#ifdef __DEBUG
                perror("rls-server: accept");
#endif
                continue;
            }

            // check if maximum number of connections reached

            if (connections >= maxconn) {
                sndack(client_socket, 50);
                close(client_socket);
                continue;
            }

            // set reading and writing timeouts

            if (setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) == -1) {
#ifdef __DEBUG
                perror("rls-server: setsockopt for reading timeout");
#endif
                close(client_socket);
                continue;
            }

            if (setsockopt(client_socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) == -1) {
#ifdef __DEBUG
                perror("rls-server: setsockopt for writing timeout");
#endif
                close(client_socket);
                continue;
            }

            printf("\n----- New client connection from %s:%d\n\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            int handler_pid = fork();
#ifdef __DEBUG
            if (handler_pid == -1) {
                perror("rls-server: fork");
            }
#endif
            if (handler_pid == 0) {
                close(server_socket);
                rlss_handler(); // never returns
            }

            if (handler_pid > 0) {
                close(client_socket);
                connections++;
            }
        }

        /* ----- user input ----- */

        else if (FD_ISSET(STDIN_FILENO, &readfds))
        {
            char *input = userinput();
            if (input == NULL)
                continue;
            
            if (strcmp(input, "quit") == 0 || input[0] == '\x04') {
                free(input);
                kill(getpid(), SIGQUIT);
            }

            free(input);
            printf("Use 'quit' to shutdown the server.\n");
        }
    }
}
