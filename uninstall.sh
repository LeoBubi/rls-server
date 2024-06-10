#!/usr/bin/env bash


# Check if the script is run as root
if [[ $EUID -ne 0 ]]; then
    echo "Uninstallation must be performed as root"
    exit 1
fi


bindir="/usr/local/bin"
mandir="/usr/local/share/man"
srcdir="/usr/local/src"
etcdir="/usr/local/etc"


# Check if rls-server is installed
if [ ! -f "$bindir/rls-server" ]; then
    echo "rls-server seems not to be installed"
    exit 0
fi


# Remove rls-server from bin directory
rm $bindir/rls-server
if [ $? -eq 0 ]; then
    echo "Removed rls-server from $bindir"
else
    echo "Cannot remove rls-server from $bindir"
    exit 1
fi


# Remove rls-server directory
rm -rf $etcdir/rls-server
if [ $? -eq 0 ]; then
    echo "Removed rls-server directory under $etcdir"
else
    echo "Cannot remove rls-server directory under $etcdir"
    exit 1
fi

# Remove rls-server man pages
rm -f $mandir/man1/rls-server.1 $mandir/man7/rls-server.7
if [ $? -eq 0 ]; then
    echo "Removed rls-server man pages under $mandir"
else
    echo "Cannot remove rls-server man pages under $mandir"
    exit 1
fi

# Remove rls-server source code
rm -rf $srcdir/rls-server
if [ $? -eq 0 ]; then
    echo "Removed rls-server source code under $srcdir"
else
    echo "Cannot remove rls-server source code under $srcdir"
    exit 1
fi

echo "rls-server has been uninstalled successfully"
exit 0

