#include "includes.h"


extern int port;       // server port number
extern int maxconn;    // maximum number of connections

int server_socket;    // server socket file descriptor
int client_socket;    // client socket file descriptor


#define destroy_zombies() while(waitpid(-1, NULL, WNOHANG) > 0)


int controlled_shutdown(int signo) {
    if (signo); // suppress warning
    close(server_socket);
    if (fcntl(client_socket, F_GETFD) != -1) {  // check if client socket is open
        sndack(client_socket, 50);
        close(client_socket);
    }
    kill(0, SIGUSR1);
    while(wait(NULL) > 0);
    _exit(EXIT_SUCCESS);
}


void
rls_listener(void)
{
    /* ----- set signal handlers ----- */

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = controlled_shutdown;
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

    /* ----- accept new client connetion ----- */

    while (1)
    {
        client_socket = accept(server_socket, NULL, NULL);
        
        if (client_socket > 0)
        {
            int handler_pid = fork();
            
            if (handler_pid == 0) {
                close(server_socket);
                rls_handler(client_socket); // never returns
            }

            if (handler_pid > 0)
                close(client_socket);
        } 

        destroy_zombies();
    }
}
