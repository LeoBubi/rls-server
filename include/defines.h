#ifndef _DEFINES_H
#define _DEFINES_H

#include <includes.h>


extern const char *CONFIG_FILE; // configuration file path


#define PORTMIN 1       // minimum server port number
#define PORTMAX 65535   // maximum server port number
#define MINCONN 1       // minimum maximum number of connections
#define MAXCONN 64      // maximum maximum number of connections

#define CLINMAX 128 // maximum line length in configuration file

#define fun_fail(msg)  { fprintf(stderr, "%s\n", msg); return 0; }

#define TXTMSG 0  // text message type code
#define SIGMSG 1  // signal message type code
#define CTLMSG 2  // control message type code

#define sig_t char  // signal type
#define ctl_t char  // control type
#define ack_t char  // ACK type

#define CTLQUIT 0  // quit command code



/**
 * @brief Read a line of text from a file descriptor.
 * @param line Buffer to store the line.
 * @param n Size of the buffer.
 * @param fd File descriptor.
 * @return 1 if successful, 0 otherwise.
*/
int rdline(char *line, size_t n, int fd);


/**
 * @brief Check if a string is an integer.
 * @param s String to check.
 * @return 1 if integer, 0 otherwise.
*/
int isint(const char *s);


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
 * @param prompt Prompt message or NULL.
 * @return User input string w/o newline character.
 * @note Returned string must be freed by the caller.
*/
char* userinput(char* prompt);


/**
 * @brief Send an ACK to a client.
 * @param fd Client socket file descriptor.
 * @param code ACK code.
 * @return 1 if successful, 0 otherwise.
*/
int sndack(int sockfd, ack_t ack);


/**
 * @brief Send a message to a client.
 * @param fd Client socket file descriptor.
 * @param msg Message to send.
 * @return 1 if successful, 0 otherwise.
*/
int sndmsg(int sockfd, const char *msg);


/**
 * @brief Get a message from a client.
 * @param fd Client socket file descriptor.
 * @param type Message type buffer.
 * @return Message string if successful, NULL otherwise.
 * @note Returned string must be freed by the caller.
*/
char* getmsg(int sockfd, char* type);


/**
 * @brief rls server initial checks.
 * @return 1 if all checks are passed, 0 otherwise.
*/
int rls_server_checks(void);


/**
 * @brief rls server setup.
 * @param argc Command line argument count.
 * @param argv Command line arguments.
 * @return 1 if successful, 0 otherwise.
*/
int rls_server_initialize(int argc, char const **argv);


/**
 * @brief rls listener process.
*/
void rls_listener(void);


/**
 * @brief rls handler process.
*/
void rls_handler(void);


/**
 * @brief reimplementation of posix_openpt.
*/
int rlsch_openpt(int flags);


/**
 * @brief reimplementation of grantpt.
*/
int rlsch_grantpt(int fd);


/**
 * @brief reimplementation of unlockpt.
*/
int rlsch_unlockpt(int fd);


/**
 * @brief reimplementation of ptsname.
*/
char* rlsch_ptsname(int fd);


#ifndef _ITOA_WORD_TYPE
# define _ITOA_WORD_TYPE	unsigned long int
#endif
char* _itoa_word (_ITOA_WORD_TYPE value, char *buflim, unsigned int base, int upper_case);





#endif /* _DEFINES_H */