kanki-irodsclient
==================

This project builds an open-source cross-platform native client for iRODS (http://www.irods.org) with a graphical user interface.
The software is implemented in C++ and builds currently against irods-4.1.x on Linux and Mac OS X 10.10 and up.

Building from source
--------------------

The following instructions should work on CentOS/RHEL 6 and 7 and probably on Fedora as well.

    yum -y groupinstall "Development Tools"
    yum -y install epel-release
    yum -y install openssl-devel libcurl-devel qt5-qtsvg-devel qt5-qtbase-devel

    # for CentOS/RHEL 6
    yum -y install ftp://ftp.renci.org/pub/irods/releases/4.1.5/centos6/irods-runtime-4.1.5-centos6-x86_64.rpm
    yum -y install ftp://ftp.renci.org/pub/irods/releases/4.1.5/centos6/irods-icommands-4.1.5-centos6-x86_64.rpm
    yum -y install ftp://ftp.renci.org/pub/irods/releases/4.1.5/centos6/irods-dev-4.1.5-centos6-x86_64.rpm

    # for CentOS/RHEL 7
    yum -y install ftp://ftp.renci.org/pub/irods/releases/4.1.5/centos7/irods-runtime-4.1.5-centos7-x86_64.rpm
    yum -y install ftp://ftp.renci.org/pub/irods/releases/4.1.5/centos7/irods-icommands-4.1.5-centos7-x86_64.rpm
    yum -y install ftp://ftp.renci.org/pub/irods/releases/4.1.5/centos7/irods-dev-4.1.5-centos7-x86_64.rpm

On Ubuntu 14.04 and up you can install build depedencies with (thanks to Paul van Schayck for this information!)

    # for Ubuntu 14.04 LTS and upwards
    sudo apt-get install qt5-qmake qtbase5-dev libqt5svg5-dev libcurl4-nss-dev qt5-default

Building the application is done simply via

    git clone https://github.com/ilarik/kanki-irodsclient.git
    cd kanki-irodsclient
    ./build.sh [ -q /usr/lib/x86_64-linux-gnu/qt5 for Ubuntu!]

You can install the binary and config into place by running

    sudo install ./src/irodsclient /usr/bin
    sudo install -D -m 644 ./src/schema.xml /etc/irods/schema.xml

Getting started
---------------

For now, irodsclient uses the same configuration as irods-icommands. In the future the client will have its 
own configuration and credentials management with the option to revert to ~/.irods/irods_environment.json.

So to be able to connect, you should have the iRODS environment configured at 

   ~/.irods/irods_environment.json

and then run

    iinit
    irodsclient &
