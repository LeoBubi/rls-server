#include "includes.h"

int 
rdline(char *line, size_t n, int fd) 
{
    ssize_t rb;     // read byte
    size_t  tb = 0; // total bytes in line
    char    rc;     // read character

    while (tb < n-1) 
    {
        rb = read(fd, &rc, 1);
        
        if (rb == -1) {
#ifdef __DEBUG
            perror("rdline: read");
#endif
            return 0;
        }

        else if (rb == 0) {
            if (tb == 0)
                return 0; // no more lines to read
            
            fun_fail("Corrupted configuration file: no newline at end of file.")
        }

        if (rc == '\n') 
        {
            if (tb == 0)
                continue; // skip empty lines

            line[tb] = '\0';
            return 1;
        }
        
        line[tb++] = rc;
    }

    fun_fail("Corrupted configuration file: line too long")
}