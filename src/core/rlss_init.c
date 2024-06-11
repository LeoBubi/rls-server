#include "includes.h"


const char *CONFIG_FILE = "/usr/local/etc/rls-server/rls-server.conf";    // configuration file path


extern int port;                // server port number
extern int maxconn;             // maximum number of connections
extern int pass_max_attempts;   // maximum number of password attempts
extern int connto;              // client communication delay limit
extern int wpdelay;             // delay between wrong password attempts


// check if port is an integer and within valid range
int __check_port(const char *port_str);
// check if maximum number of connections is an integer and within valid range
int __check_maxconn(const char *maxconn_str);



int
rlss_init(int argc, char const **argv)
{
    /* ----- set effective uid to user for security ----- */

    seteuid(getuid());  // ruid = euid = caller uid -- suid = root uid
    
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
            
            if (!__check_port(argv[i+1]))
                return 0;

            port = atoi(argv[++i]);
        }

        // check for maximum number of connections option
        else if (strcmp(argv[i], "-m") == 0) 
        {
            if (i+1 >= argc)
                fun_fail("No maximum number of connections specified.");
            
            if (!__check_maxconn(argv[i+1]))
                return 0;

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
        if (!config_get("SRVPORT", port_str, 6))
            fun_fail("Failed to get port from configuration file.")
        
        if (!__check_port(port_str))
            return 0;
        
        port = atoi(port_str);
    }

    // if maximum number of connections not provided, get from configuration file
    if (maxconn == 0)
    {
        char maxconn_str[16];
        if (!config_get("MAXCONN", maxconn_str, 4))
            fun_fail("Failed to get maximum number of connections from configuration file.")
        
        if (!__check_maxconn(maxconn_str))
            return 0;
        
        maxconn = atoi(maxconn_str);
    }

    // get password maximum attempts from configuration file
    char pass_max_attempts_str[16];
    if (!config_get("WPATTPT", pass_max_attempts_str, 16))
        fun_fail("Failed to get maximum password attempts from configuration file.")
    
    if (!isint(pass_max_attempts_str))
        fun_fail("Maximum password attempts in configuration file must be an integer.")
    if (atoi(pass_max_attempts_str) < 1)
        fun_fail("Maximum password attempts must be at least 1.")
    
    pass_max_attempts = atoi(pass_max_attempts_str);


    // get client communication delay limit from configuration file
    char connto_str[16];
    if (!config_get("CONNTMO", connto_str, 16))
        fun_fail("Failed to get client communication delay limit from configuration file.")
    
    if (!isint(connto_str))
        fun_fail("Maximum password attempts in configuration file must be an integer.")
    if (atoi(connto_str) < MINCNTO)
        fun_fail("Client communication delay limit in configuration file too low.")
    if (atoi(connto_str) > MAXCNTO)
        fun_fail("Client communication delay limit in configuration file too high.")
    
    connto = atoi(connto_str);


    // get delay between wrong password attempts from configuration file
    char wpdelay_str[16];
    if (!config_get("WPDELAY", wpdelay_str, 16))
        fun_fail("Failed to get delay between wrong password attempts from configuration file.")
    
    if (!isint(wpdelay_str))
        fun_fail("Delay between wrong password attempts in configuration file must be an integer.")
    if (atoi(wpdelay_str) < MINDLAY)
        fun_fail("Delay between wrong password attempts in configuration file too low.")
    if (atoi(wpdelay_str) > MAXDLAY)
        fun_fail("Delay between wrong password attempts in configuration file too high.")

    wpdelay = atoi(wpdelay_str);


    return 1;
}


int 
__check_port(const char *port_str)
{
    if (!isint(port_str))
        fun_fail("Port must be an integer.")
    if (atoi(port_str) < PORTMIN) {
        fprintf(stderr, "Minimum port number is %d.\n", PORTMIN);
        return 0;
    }
    if (atoi(port_str) > PORTMAX) {
        fprintf(stderr, "Maximum port number is %d.\n", PORTMAX);
        return 0;
    }
    
    return 1;
}


int
__check_maxconn(const char *maxconn_str)
{
    if (!isint(maxconn_str))
        fun_fail("Maximum number of connections must be an integer.")
    if (atoi(maxconn_str) < MINCONN) {
        fprintf(stderr, "Minimum maximum number of connections is %d.\n", MINCONN);
        return 0;
    }
    if (atoi(maxconn_str) > MAXCONN) {
        fprintf(stderr, "Maximum maximum number of connections is %d.\n", MAXCONN);
        return 0;
    }
    
    return 1;
}

