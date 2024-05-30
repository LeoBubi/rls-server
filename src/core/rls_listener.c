#include "includes.h"


extern int port;       // server port number
extern int maxconn;    // maximum number of connections

int server_socket;    // server socket file descriptor
int client_socket;    // client socket file descriptor


int controlled_shutdown(int signo) {
    if (signo); // suppress warning
    close(server_socket);
    if (fcntl(client_socket, F_GETFD) != -1) {  // check if client socket is open
        sndack(client_socket, 50);
    }
}


void
rls_listener(void)
{
    /* ----- set signal handlers ----- */

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));


    /* ----- start listening for client connections ------ */



    /* ----- accept new client connetion ----- */
}
