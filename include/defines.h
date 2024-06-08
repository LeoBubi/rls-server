#ifndef _DEFINES_H
#define _DEFINES_H

#include <includes.h>


extern const char *CONFIG_FILE; // configuration file path


#define PORTMIN 1       // minimum server port number
#define PORTMAX 65535   // maximum server port number
#define MINCONN 1       // minimum maximum number of connections
#define MAXCONN 64      // maximum maximum number of connections
#define MINCNTO 1       // minimum communication delay limit (seconds)
#define MAXCNTO 3600    // maximum communication delay limit (seconds)
#define MINDLAY 1       // minimum delay between wrong password attempts (seconds)
#define MAXDLAY 5       // maximum delay between wrong password attempts (seconds)

#define fun_fail(msg)  { fprintf(stderr, "%s\n", msg); return 0; }

#define TXTMSG 0  // text message type code
#define SIGMSG 1  // signal message type code
#define CTLMSG 2  // control message type code
#define CHRMSG 3  // character message type code
#define CLOSED 4  // closed message type code

#define rlssig_t char  // signal type
#define rlsctl_t char  // control type
#define rlsack_t char  // ACK type

#define CTLQUIT 0  // quit command code



/**
 * @brief Check if a string is an integer.
 * @param s String to check.
 * @return 1 if integer, 0 otherwise.
*/
int isint(const char *s);


/**
 * @brief Read a line from configuration file
 * @param fd Configuration file descriptor.
 * @return Line string if successful, NULL otherwise.
 * @note Returned string must be freed by the caller.
*/
char* rdline(int fd);


/**
 * @brief Get configuration value from configuration file.
 * @param key Configuration key.
 * @param value Configuration value buffer.
 * @param n Size of the value buffer.
 * @return 1 if successful, 0 otherwise.
*/
int config_get(char *key, char *value, size_t n);


/**
 * @brief Get user input.
 * @return User input string w/o newline character.
 * @note Returned string must be freed by the caller.
*/
char* userinput(void);


/**
 * @brief Send an ACK to a client.
 * @param fd Client socket file descriptor.
 * @param code ACK code.
 * @return 1 if successful, 0 otherwise.
*/
int sndack(int sockfd, rlsack_t ack);


/**
 * @brief Send a character to a client.
 * @param fd Client socket file descriptor.
 * @param c Character to send.
 * @return 1 if successful, 0 otherwise.
*/
int sndchr(int sockfd, char c);


/**
 * @brief Get a message from a client.
 * @param fd Client socket file descriptor.
 * @param type Message type buffer.
 * @return Message string if successful, NULL otherwise.
 * @note Returned string must be freed by the caller.
*/
char* getmsg(int sockfd, char* type);


/**
 * @brief Create a new pseudo-terminal file descriptor pair.
 * @param amaster Master file descriptor.
 * @param aslave Slave file descriptor.
 * @return 1 if successful, 0 otherwise.
*/
int ptypair(int* amaster, int* aslave);


/**
 * @brief Things to check before anything else.
 * @return 1 if all checks are passed, 0 otherwise.
*/
int rlss_check(void);


/**
 * @brief Import command line and configuration parameters.
 * @param argc Command line argument count.
 * @param argv Command line arguments.
 * @return 1 if successful, 0 otherwise.
*/
int rlss_init(int argc, char const **argv);


/**
 * @brief rls handler process.
*/
void rlss_handler(void);


#endif /* _DEFINES_H */