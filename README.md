# rls-server

**rls-server** is a server that allows (remote) rls clients to connect to and obtain a shell on this host.

### Installation

To install **rls-server**, please run the installation script **install.sh** with root privileges.

The provided Makefile has to be used only by the installation script, so refrain from running **make** manually.

To run **rls-server** in debugging mode (i.e. with extended verbosity) please add
```
-D __DEBUG
```
to the `CFLAGS` in the Makefile.

### Uninstallation

To uninstall **rls-server**, please run the uninstallation script **uninstall.sh** with root privileges.

### Manual

All the necessary documentation is provided in the **rls-server** manual pages.

`rls-server(1)` provides usage information.

`rls-server(7)` provides details about the internal structure and functioning of the server.

These manual pages are installed in `/usr/local/share/man`. Please make sure to have it in your **MANPATH**.
