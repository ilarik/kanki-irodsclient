kanki-irodsclient
==================

This project builds an open-source cross-platform native client for iRODS (http://www.irods.org) with a graphical user interface.
The software is implemented in C++ and builds currently against irods-4.1.x on Linux and Mac OS X 10.10. Binary distributions are provided for CentOS/RHEL 6 and 7 and for Mac OS X 10.10 and 10.11.

Installation 
-------------

In CentOS/RHEL Linux distributions the installation of the binary packages is very straightforward. 

    yum -y install epel-release
    yum -y install ftp://ftp.renci.org/pub/irods/releases/4.1.6/centos6/irods-runtime-4.1.6-centos6-x86_64.rpm
    yum -y install ftp://ftp.renci.org/pub/irods/releases/4.1.6/centos6/irods-icommands-4.1.6-centos6-x86_64.rpm
    yum -y install https://github.com/ilarik/kanki-irodsclient/releases/download/1.0.7/kanki-irodsclient-1.0.7-1.el6.x86_64.rpm

In OS X you need to download the OS X .pkg file from https://github.com/ilarik/kanki-irodsclient/releases and right-click the download to open contextual menu from which selecting open will prompt you for confirmation to install an unsigned package (as I don't have an Apple Developer ID yet).


Getting started
---------------

For now, irodsclient uses the same configuration as irods-icommands. In the future the client will have its 
own configuration and credentials management with the option to revert to iRODS environment.

So to be able to connect, you should have the iRODS environment configured at 

`~/.irods/irods_environment.json`

and then run

    iinit
    irodsclient &

In OS X after the package install process is finished you have the irodsclient and a full distribution of irods-icommands at /Applications/iRODS.app. To configure the iRODS environment you need to update your shell configuration as follows (for bash, zsh etc. Bourne-derived shells)

    export PATH=/Applications/iRODS.app/Contents/PlugIns/irods/icommands:$PATH
    export DYLD_LIBRARY_PATH=/Applications/iRODS.app/Contents/Frameworks:$DYLD_LIBRARY_PATH

In your iRODS environment file at ~/.irods/irods_environment.json you need to define

    "irods_plugins_home": "/Applications/iRODS.app/Contents/PlugIns/irods/"

That concludes the install procedure, running `iinit` will initialize your iRODS commandline and GUI environment.



Building from source
--------------------

The following instructions should work on CentOS/RHEL 6 and 7 and probably on Fedora as well.

    yum -y groupinstall "Development Tools"
    yum -y install epel-release
    yum -y install openssl-devel libcurl-devel qt5-qtsvg-devel qt5-qtbase-devel

    # for example for CentOS/RHEL 6
    yum -y install ftp://ftp.renci.org/pub/irods/releases/4.1.6/centos6/irods-runtime-4.1.6-centos6-x86_64.rpm
    yum -y install ftp://ftp.renci.org/pub/irods/releases/4.1.6/centos6/irods-icommands-4.1.6-centos6-x86_64.rpm
    yum -y install ftp://ftp.renci.org/pub/irods/releases/4.1.6/centos6/irods-dev-4.1.6-centos6-x86_64.rpm

On Ubuntu 14.04 and up you can install build depedencies with (thanks to Paul van Schayck for this information!)

    # for Ubuntu 14.04 LTS and upwards
    sudo apt-get install qt5-qmake qtbase5-dev libqt5svg5-dev libcurl4-nss-dev qt5-default

Building the application is done simply via

    git clone https://github.com/ilarik/kanki-irodsclient.git
    cd kanki-irodsclient
    ./build.sh [ -q /usr/lib/x86_64-linux-gnu/qt5 for Ubuntu! ]

You can install the binary and config into place by running

    sudo install ./src/irodsclient /usr/bin
    sudo install -D -m 644 ./src/schema.xml /etc/irods/schema.xml
