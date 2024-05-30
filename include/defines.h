#ifndef _DEFINES_H
#define _DEFINES_H

#include <includes.h>


#define CONFIG_FILE "/etc/rls-server/rls-server.conf"


#define PORTMIN 1       // minimum server port number
#define PORTMAX 65535   // maximum server port number
#define MINCONN 1       // minimum maximum number of connections
#define MAXCONN 64      // maximum maximum number of connections
#define CONNLEN 2       // number of digits in maximum number of connections

#define CLINMAX 128 // maximum line length in configuration file

#define fun_fail(msg)  { fprintf(stderr, "%s\n", msg); return 0; }

#define TXTMSG 0  // text message type code
#define SIGMSG 1  // signal message type code
#define CTLMSG 2  // control message type code

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





#endif /* _DEFINES_H */