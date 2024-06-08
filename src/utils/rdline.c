#include "includes.h"

#define RDLN_INITBUFSIZ 32    // initial size of line buffer
#define RDLN_MAXBUFSIZ  4096  // maximum size of line buffer

char* 
rdline(int fd) 
{
    size_t line_maxsize = RDLN_INITBUFSIZ;  // initial size of line buffer
    size_t line_size    = 0;               // initial size of line

    // allocate initial buffer for the line string
    char *line = (char*)malloc(line_maxsize);
    if (!line) {
#ifdef __DEBUG
        perror("rdline: malloc");
#endif
        return NULL;
    }

    char c;
    ssize_t br;
    while (1)
    {
        while ((br = read(fd, &c, 1)) > 0 && c != '\n') 
        {
            if (line_size == RDLN_MAXBUFSIZ) {
#ifdef __DEBUG
                fprintf(stderr, "rdline: line too long\n");
#endif
                free(line);
                return NULL;
            }

            if (line_size == line_maxsize)
            {
                line_maxsize *= 2;
                char *tmp = (char*)realloc(line, line_maxsize);
                if (!tmp) {
#ifdef __DEBUG
                    perror("rdline: realloc");
#endif
                    free(line);
                    return NULL;
                }
                line = tmp;
            }

            line[line_size++] = c;
        }

        if (br == -1) {
#ifdef __DEBUG
            perror("rdline: read");
#endif
            free(line);
            return NULL;
        }

        if (br == 0) 
        {
            if (line_size == 0) {
                free(line);
                return NULL;    // no lines to read
            }
            
#ifdef __DEBUG
            fprintf(stderr, "rdline: no newline at end of file.\n");
#endif
            free(line);
            return NULL;
            
        }

        if (line_size == 0)
            continue; // skip empty lines

        line[line_size] = '\0'; // null-terminate the line string

        return line;
    }
}