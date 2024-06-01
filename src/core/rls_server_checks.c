#include "includes.h"

int
rls_server_checks(void)
{
    /* ----- Check RESuid ----- */

    uid_t ruid, euid, suid;

    getresuid(&ruid, &euid, &suid);

    // real uid must NOT be root
    if (ruid == 0)
        fun_fail("Running as root is unnacessary and hence not allowed.");

    // program must have SETUID bit set
    if (euid != 0 || suid != 0)
        fun_fail("Program not configured properly. Please uninstall and reinstall.");
    
    return 1;
}