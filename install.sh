#!/usr/bin/env bash


# Check if the script is run as root
if [[ $EUID -ne 0 ]]; then
    echo "Installation must be performed as root"
    exit 1
fi


bindir="/usr/local/bin"
mandir="/usr/local/share/man"
srcdir="/usr/local/src"
etcdir="/usr/local/etc"


# Check if rls-server is installed already
if [ -f "$bindir/rls-server" ]; then
    echo "rls-server seems to be installed already"
    exit 0
fi


bin=1
man=1
src=1
etc=1
missing=0


# Check if bin directory exists
if [ ! -d $bindir ]; then
    bin=0
    missing=1
fi

# Check if man directory exists
if [ ! -d $mandir ]; then
   man=0
   missing=1
fi

# Check if src directory exists
if [ ! -d $srcdir ]; then
    src=0
    missing=1
fi

#check if etc directory exists
if [ ! -d $etcdir ]; then
    etc=0
    missing=1
fi

if [ $missing -eq 1 ]; then
    echo "The following directories are going to be created:"
    if [ $bin -eq 0 ]; then
        echo "    $bindir"
    fi
    if [ $man -eq 0 ]; then
        echo "    $mandir"
    fi
    if [ $src -eq 0 ]; then
        echo "    $srcdir"
    fi
    if [ $etc -eq 0 ]; then
        echo "    $etcdir"
    fi
    echo "Continue? (Y/n)"
    
    read -n 1 ans
    if [ "$ans" != "y" -a "$ans" != "Y" ]; then
        echo "Installation aborted"
        exit 0
    fi

    if [ $bin -eq 0 ]; then
        mkdir -p $bindir
        if [ $? -eq 0 ]; then
            echo "Created $bindir"
        else
            echo "Failed to create $bindir"
            exit 1
        fi
    fi
    if [ $man -eq 0 ]; then
        mkdir -p $mandir
        if [ $? -eq 0 ]; then
            echo "Created $mandir"
        else
            echo "Failed to create $mandir"
            exit 1
        fi
    fi
    if [ $src -eq 0 ]; then
        mkdir -p $srcdir
        if [ $? -eq 0 ]; then
            echo "Created $srcdir"
        else
            echo "Failed to create $srcdir"
            exit 1
        fi
    fi
    if [ $etc -eq 0 ]; then
        mkdir -p $etcdir
        if [ $? -eq 0 ]; then
            echo "Created $etcdir"
        else
            echo "Failed to create $etcdir"
            exit 1
        fi
    fi
fi
    


# Compile rls-server
echo ""
echo "Compiling rls-server ..."
make
if [ $? -ne 0 ]; then
    echo "Failed to compile rls-server"
    exit 1
fi
echo "... done"
echo ""

# Install rls-server in bin directory
mv rls-server $bindir/rls-server
if [ $? -eq 0 ]; then
    echo "Installed rls-server to $bindir"
else
    echo "Failed to install rls-server to $bindir"
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
mkdir -p $etcdir/rls-server

if [ $? -eq 0 ]; then
    echo "Created rls-server directory under $etcdir"
else
    echo "Failed to create rls-server directory"
    exit 1
fi


# Create rls-server default configuration file
touch $etcdir/rls-server/rls-server.conf
if [ $? -eq 0 ]; then
    echo "Created rls-server configuration file under $etcdir/rls-server"
else
    echo "Failed to create rls-server configuration file"
    exit 1
fi

# Initialize rls-server configuration file
echo "SRVPORT=513" > $etcdir/rls-server/rls-server.conf
echo "MAXCONN=16" >> $etcdir/rls-server/rls-server.conf
echo "WPATTPT=3" >> $etcdir/rls-server/rls-server.conf
echo "WPDELAY=2" >> $etcdir/rls-server/rls-server.conf
echo "CONNTMO=10" >> $etcdir/rls-server/rls-server.conf

if [ $? -eq 0 ]; then
    echo "Initialized rls-server configuration file"
else
    echo "Failed to initialize rls-server configuration file"
    exit 1
fi

# Move rls-server(1) to man directory
if [ ! -d "$mandir/man1" ]; then
    mkdir $mandir/man1
    if [ $? -eq 0 ]; then
        echo "Created $mandir/man1 directory"
    else
        echo "Failed to create $mandir/man1 directory"
        exit 1
    fi
fi
cp man/rls-server.1 $mandir/man1
if [ $? -eq 0 ]; then
    echo "Moved rls-server(1) to $mandir/man1"
else
    echo "Failed to move rls-server(1) to $mandir/man1"
    exit 1
fi

# Move rls-server(7) to /usr/local/share/man/man7
if [ ! -d "$mandir/man7" ]; then
    mkdir $mandir/man7
    if [ $? -eq 0 ]; then
        echo "Created $mandir/man7 directory"
    else
        echo "Failed to create $mandir/man7 directory"
        exit 1
    fi
fi
cp man/rls-server.7 $mandir/man7
if [ $? -eq 0 ]; then
    echo "Moved rls-server(7) to $mandir/man7"
else
    echo "Failed to move rls-server(7) to $mandir/man7"
    exit 1
fi

# Copy source code to src directory
mkdir $srcdir/rls-server
if [ $? -eq 0 ]; then
    echo "Created $srcdir/rls-server"
else
    echo "Failed to create $srcdir/rls-server"
    exit 1
fi
cp -r include $srcdir/rls-server/include
cp -r src $srcdir/rls-server/src
if [ $? -eq 0 ]; then
    echo "Copied source code to $srcdir/rls-server"
else
    echo "Failed to copy source code to $srcdir/rls-server"
    exit 1
fi


echo ""
echo "rls-server has been installed successfully"
echo ""
echo "    Default port: 513"
echo "    Default maximum number of connections: 16"
echo "    Defualt maximum client password attempts: 3"
echo "    Default client communication delay limit: 10 seconds"
echo "    Default delay between wrong password attempts: 2 seconds"
echo ""

exit 0

