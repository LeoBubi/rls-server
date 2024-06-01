#include "includes.h"


extern const char *CONFIG_FILE;  // configuration file path


int
config_get(char *key, char *value, size_t n)
{
    int config_fd = open(CONFIG_FILE, O_RDONLY);
    if (config_fd == -1) {
#ifdef __DEBUG
        perror("config_get: open");
        return 0;
#else
        fun_fail("Failed to open configuration file.")
#endif
    }
    
    char line[CLINMAX +1]; // +1 for null terminator
    while (rdline(line, CLINMAX+1, config_fd))
    {
        char *tok = strtok(line, "=");
        if (tok == NULL) {
            close(config_fd);
            fun_fail("Invalid configuration file format.")
        }
        
        if (strcmp(tok, key) == 0)
        {
            tok = strtok(NULL, "\0");
            if (tok == NULL) {
                close(config_fd);
                fun_fail("Invalid configuration file format: no value for specified key.")
            }
            
            if (strlen(tok) > n-1) {
                close(config_fd);
                fun_fail("Invalid configuration file format: value too long.")
            }
            
            close(config_fd);
            strncpy(value, tok, n);
            return 1;
        }
    }

    close(config_fd);
    fun_fail("Key not found in configuration file.")
}
