#!/usr/bin/env bash


# Check if the script is run as root
if [[ $EUID -ne 0 ]]; then
    echo "Uninstallation must be performed as root"
    exit 1
fi


# Check if rls-server is installed
if [ ! -f "/usr/local/bin/rls-server" ]; then
    echo "rls-server seems not to be installed"
    exit 0
fi


# Remove rls-server from /usr/local/bin
rm /usr/local/bin/rls-server
if [ $? -eq 0 ]; then
    echo "Removed rls-server from /usr/local/bin"
else
    echo "Cannot remove rls-server from /usr/local/bin"
    exit 1
fi


# Remove rls-server directory
rm -rf /etc/rls-server
if [ $? -eq 0 ]; then
    echo "Removed rls-server directory under /etc"
else
    echo "Cannot remove rls-server directory under /etc"
    exit 1
fi

echo "rls-server has been uninstalled successfully"
exit 0

