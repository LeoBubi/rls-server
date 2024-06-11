#include "includes.h"

int
rlss_check(void)
{
    /* ----- Check RESuid ----- */

    uid_t ruid, euid, suid;

    getresuid(&ruid, &euid, &suid);

    // real uid must NOT be root
    if (ruid == 0)
        fun_fail("Running as root is unnacessary and hence not allowed.");

    // program must have SETUID bit set
    if (euid != 0 || suid != 0)
        fun_fail("Program not configured properly. Please reinstall it.");
    
    return 1;

    /* ----- check configuration file ----- */

    if (access(CONFIG_FILE, F_OK) == -1)
        fun_fail("Configuration file not found.");

    if (access(CONFIG_FILE, R_OK) == -1)
        fun_fail("Configuration file not readable.");
}