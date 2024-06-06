#include "includes.h"


const char *CONFIG_FILE = "/etc/rls-server/rls-server.conf";    // configuration file path


extern int pass_max_attempts;  // maximum number of password attempts

int port;       // server port number
int maxconn;    // maximum number of connections
int connto;     // client communication delay limit


int
rls_server_initialize(int argc, char const **argv)
{
    /* ----- set effective uid to user for security ----- */

    seteuid(getuid());  // ruid = euid = caller uid -- suid = root uid

    /* ----- check configuration file ----- */

    if (access(CONFIG_FILE, F_OK) == -1)
        fun_fail("Configuration file not found.");

    if (access(CONFIG_FILE, R_OK) == -1)
        fun_fail("Configuration file not readable.");
    
    /* ----- read command line arguments ----- */

    port    = 0;    // initialize port number to 0
    maxconn = 0;    // initialize maximum number of connections to 0

    for (int i = 1; i < argc; i++) 
    {
        // check for port option
        if (strcmp(argv[i], "-p") == 0) 
        {
            if (i+1 >= argc)
                fun_fail("No port number specified.");
            if (!isint(argv[i+1]))
                fun_fail("Port number must be an integer.");
            if (atoi(argv[i+1]) < PORTMIN) {
                fprintf(stderr, "Minimum port number is %d.\n", PORTMIN);
                return 0;
            }
            if (atoi(argv[i+1]) > PORTMAX) {
                fprintf(stderr, "Maximum port number is %d.\n", PORTMAX);
                return 0;
            }

            port = atoi(argv[++i]);
        }

        // check for maximum number of connections option
        else if (strcmp(argv[i], "-m") == 0) 
        {
            if (i+1 >= argc)
                fun_fail("No maximum number of connections specified.");
            if (!isint(argv[i+1]))
                fun_fail("Maximum number of connections must be an integer.");
            if (atoi(argv[i+1]) < MINCONN) {
                fprintf(stderr, "Minimum maximum number of connections is %d.\n", MINCONN);
                return 0;
            }
            if (atoi(argv[i+1]) > MAXCONN) {
                fprintf(stderr, "Maximum maximum number of connections is %d.\n", MAXCONN);
                return 0;
            }

            maxconn = atoi(argv[++i]);
        }

        // invalid argument
        else {
            fprintf(stderr, "%s: Invalid argument.\n", argv[i]);
            return 0;
        }
    }

    /* ----- read configuration file ----- */

    // if port not provided, get from configuration file
    if (port == 0)
    {
        char port_str[6]; // 5 digits + null terminator
        if (!config_get("PORT", port_str, 6))
            fun_fail("Failed to get port from configuration file.")
        
        if (!isint(port_str))
            fun_fail("Port number in configuration file must be an integer.")
        if (atoi(port_str) < PORTMIN)
            fun_fail("Port number in configuration file too low.")
        if (atoi(port_str) > PORTMAX)
            fun_fail("Port number in configuration file too high.")
        
        port = atoi(port_str);
    }

    // if maximum number of connections not provided, get from configuration file
    if (maxconn == 0)
    {
        char maxconn_str[16];
        if (!config_get("MAXCONN", maxconn_str, 4))
            fun_fail("Failed to get maximum number of connections from configuration file.")
        
        if (!isint(maxconn_str))
            fun_fail("Maximum number of connections in configuration file must be an integer.")
        if (atoi(maxconn_str) < MINCONN)
            fun_fail("Maximum number of connections in configuration file too low.")
        if (atoi(maxconn_str) > MAXCONN)
            fun_fail("Maximum number of connections in configuration file too high.")
        
        maxconn = atoi(maxconn_str);
    }

    // get password maximum attempts from configuration file
    char pass_max_attempts_str[16];
    if (!config_get("PASSATTEMPTS", pass_max_attempts_str, 16))
        fun_fail("Failed to get maximum password attempts from configuration file.")
    
    if (!isint(pass_max_attempts_str))
        fun_fail("Maximum password attempts in configuration file must be an integer.")
    if (atoi(pass_max_attempts_str) < 1)
        fun_fail("Maximum password attempts must be at least 1.")
    
    pass_max_attempts = atoi(pass_max_attempts_str);


    // get client communication delay limit from configuration file
    char connto_str[16];
    if (!config_get("CONNTIMEO", connto_str, 16))
        fun_fail("Failed to get client communication delay limit from configuration file.")
    
    if (!isint(connto_str))
        fun_fail("Maximum password attempts in configuration file must be an integer.")
    if (atoi(connto_str) < 1)
        fun_fail("Maximum password attempts must be at least 1.")
    
    connto = atoi(connto_str);


    return 1;
}
