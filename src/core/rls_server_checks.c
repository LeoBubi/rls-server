#include "includes.h"

int
rls_server_checks(void)
{
    /* ----- make sure only one rls-server instance is running ----- */

    if (open("/tmp/rls_server_running_already", O_RDONLY | O_CREAT | O_EXCL, 00600) == -1) {
        if (errno = EEXIST)
            fprintf(stderr, "rls-server is already active.\n");
        else {
#ifdef __DEBUG
            perror("rls-server: open");
#else
            fprintf(stderr, "Unable to start rls-server.\n");
#endif
        }
        exit(EXIT_FAILURE);
    }

    if (unlink("/tmp/rls_server_running_already") == -1) {
#ifdef __DEBUG
        perror("rls-server: unlink");
#endif
        fprintf(stderr, "FATAL ERROR: PLEASE REMOVE /tmp/rls_server_running_already");
        exit(EXIT_FAILURE);
    }

    /* ----- Check RESuid ----- */

    uid_t ruid, euid, suid;

    getresuid(&ruid, &euid, &suid);
    printf("%d\t%d\t%d\n", ruid, euid, suid);

    // real uid must NOT be root
    if (ruid == 0)
        fun_fail("Running as root is unnacessary and hence not allowed.");

    // program must have SETUID bit set
    if (euid != 0 || suid != 0)
        fun_fail("Program not configured properly. Please uninstall and reinstall.");
    
    return 1;
}