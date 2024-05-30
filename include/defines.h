#ifndef _DEFINES_H
#define _DEFINES_H

#include <includes.h>


#define PORTMIN  1      // minimum server port number
#define PORTMAX  65535  // maximum server port number

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



#endif /* _DEFINES_H */