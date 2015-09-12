kanki-irodsclient
==================

This project builds an open-source cross-platform native client for iRODS (http://www.irods.org) with a graphical user interface.
The software is implemented in C++ and builds currently against irods-4.0.x and irods-4.1.x on Linux and Mac OS X 10.8 and up.

Building from source
--------------------

The following instructions should work on CentOS/RHEL 6 and 7 and probably on Fedora as well. Unbuntu goes similarly (TODO).

    yum -y groupinstall "Development Tools"
    yum -y install epel-release
    yum -y install openssl-devel libcurl-devel qt5-qtsvg-devel qt5-qtbase-devel tar

    # for CentOS/RHEL 6
    yum -y install ftp://ftp.renci.org/pub/irods/releases/4.1.5/centos6/irods-runtime-4.1.5-centos6-x86_64.rpm
    yum -y install ftp://ftp.renci.org/pub/irods/releases/4.1.5/centos6/irods-dev-4.1.5-centos6-x86_64.rpm

    # for CentOS/RHEL 7
    yum -y install ftp://ftp.renci.org/pub/irods/releases/4.1.5/centos7/irods-runtime-4.1.5-centos7-x86_64.rpm
    yum -y install ftp://ftp.renci.org/pub/irods/releases/4.1.5/centos7/irods-dev-4.1.5-centos7-x86_64.rpm

    git clone https://github.com/ilarik/kanki-irodsclient.git
    cd kanki-irodsclient
    ./build.sh

    sudo install ./src/irodsclient /usr/bin
    sudo install -D -m 644 ./src/schema.xml /etc/irods/schema.xml
