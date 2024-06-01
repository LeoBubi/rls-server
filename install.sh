#!/usr/bin/env bash


# Check if the script is run as root
if [[ $EUID -ne 0 ]]; then
    echo "Installation must be performed as root"
    exit 1
fi


# Check if /usr/local/bin exists
if [ ! -d "/usr/local/bin" ]; then
    echo "Error: /usr/local/bin directory does not exist"
    exit 1
fi


# Check if rls-server is installed already
if [ -f "/usr/local/bin/rls-server" ]; then
    echo "rls-server seems to be installed already"
    exit 0
fi


# Compile rls-server
make
if [ $? -ne 0 ]; then
    echo "Failed to compile rls-server"
    exit 1
fi

# Copy rls-server to /usr/local/bin
mv rls-server /usr/local/bin
if [ $? -eq 0 ]; then
    echo "Installed rls-server to /usr/local/bin"
else
    echo "Failed to install rls-server to /usr/local/bin"
    exit 1
fi

# Elevate permissions of rls-server
chmod u+s /usr/local/bin/rls-server
if [ $? -eq 0 ]; then
    echo "Elevated rls-server permissions."
else
    echo "Failed to elevate rls-server permissions"
    exit 1
fi

# Remove object files
make cleaninstall
if [ $? -eq 0 ]; then
    echo "Removed object files"
else
    echo "Failed to remove object files"
    exit 1
fi


# Create rls-server directory
mkdir -p /etc/rls-server

if [ $? -eq 0 ]; then
    echo "Created rls-server directory under /etc"
else
    echo "Failed to create rls-server directory"
    exit 1
fi


# Create rls-server default configuration file
touch /etc/rls-server/rls-server.conf
if [ $? -eq 0 ]; then
    echo "Created rls-server configuration file under /etc/rls-server"
else
    echo "Failed to create rls-server configuration file"
    exit 1
fi

# Initialize rls-server configuration file
echo "PORT=513" > /etc/rls-server/rls-server.conf
echo "MAXCONN=16" >> /etc/rls-server/rls-server.conf
echo "PASSATTEMPTS=3" >> /etc/rls-server/rls-server.conf

if [ $? -eq 0 ]; then
    echo "Initialized rls-server configuration file"
else
    echo "Failed to initialize rls-server configuration file"
    exit 1
fi

echo "rls-server has been installed successfully"
echo ""
echo "    Default port: 513"
echo "    Default maximum number of connections: 16"
echo "    Defualt maximum client password attempts: 3"
echo ""

exit 0
