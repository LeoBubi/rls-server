#include "includes.h"


extern int port;       // server port number
extern int maxconn;    // maximum number of connections

int server_socket;    // server socket file descriptor
int client_socket;    // client socket file descriptor


#define destroy_zombies() while(waitpid(-1, NULL, WNOHANG) > 0)


void listener_shutdown(int signo) {
    if (signo){;} // suppress warning
    write(STDOUT_FILENO, "Shutting down listener...\n", sizeof("Shutting down listener...\n"));
    close(server_socket);
    // close client socket if open
    if (fcntl(client_socket, F_GETFD) != -1) {
        sndack(client_socket, 50);
        close(client_socket);
    }
    seteuid(0);     // gain root privileges
    kill(0, SIGUSR1);
    seteuid(getuid());  // drop root privileges
    while(wait(NULL) > 0){;}
    _exit(EXIT_SUCCESS);
}


void
rls_listener(void)
{
    /* ----- set signal handlers ----- */

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = listener_shutdown;
    sigaction(SIGUSR1, &sa, NULL);


    /* ----- start listening for client connections ------ */

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
#ifdef __DEBUG
        perror("rls_listener: socket");
#else
        fprintf(stderr, "Unable to start listening for client connections.\n");
#endif
        kill(getppid(), SIGUSR2);
        _exit(EXIT_FAILURE);
    }

    seteuid(0);     // gain root privileges

    // bind socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
#ifdef __DEBUG
        perror("rls_listener: bind");
#else
        fprintf(stderr, "Unable to start listening for client connections.\n");
#endif
        kill(getppid(), SIGUSR2);
        _exit(EXIT_FAILURE);
    }

    seteuid(getuid());  // drop root privileges

    // make socket passive
    if (listen(server_socket, (maxconn < SOMAXCONN ? maxconn : SOMAXCONN)) == -1) {
#ifdef __DEBUG
        perror("rls_listener: listen");
#else
        fprintf(stderr, "Unable to start listening for client connections.\n");
#endif
        kill(getppid(), SIGUSR2);
        _exit(EXIT_FAILURE);
    }

    printf("Listening for client connections on port %d...\n", port);

    /* ----- accept new client connetion ----- */

    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
#ifdef __DEBUG
        if (client_socket == -1) {
            perror("rls_listener: accept");
        }
#endif

        // set reading and writing timeouts
        struct timeval timeout;
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;

        if (setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) == -1) {
#ifdef __DEBUG
            perror("rls_listener: setsockopt for reading timeout");
#endif
            close(client_socket);
            continue;
        }

        if (setsockopt(client_socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) == -1) {
#ifdef __DEBUG
            perror("rls_listener: setsockopt for writing timeout");
#endif
        }
        
        if (client_socket > 0)
        {
            printf("\n----- New client connection from %s:%d\n\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            int handler_pid = fork();
#ifdef __DEBUG
            if (handler_pid == -1) {
                perror("rls_listener: fork");
            }
#endif
            
            if (handler_pid == 0) {
                close(server_socket);
                rls_handler(); // never returns
            }

            if (handler_pid > 0)
                close(client_socket);
        } 

        destroy_zombies();
    }
}
