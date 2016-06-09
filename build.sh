#!/bin/bash

show_help() {
echo "build.sh - build irodsclient package for OS X and Linux"
echo "usage: ./build.sh [-h] [-v] [-r os-release-label] [-q qt-root-dir]"
echo "-h prints help"
echo "-v sets verbose mode on"
echo "-r sets os release label in build package"
echo "-q sets Qt root directory path"
}

# initialize parameters with defaults
PLATFORM=`uname`
VERSION=1.1.0b2
OSRELEASE=unknown

# reset POSIX option index variable
OPTIND=1

# parse command line arguments
while getopts "hvr:q:" opt; do
    case "$opt" in
	h)
	    show_help
	    exit 0
	    ;;
	v)
	    VERBOSE=1
	    ;;
	r)
	    OSRELEASE=$OPTARG
	    ;;
	q)
	    QTROOT=$OPTARG
	    ;;
    esac
done

if [ "$PLATFORM" = "Darwin" ]; then
    echo "Building for OS X"

    # if no Qt path was provided, set to default
    if [ "$QTROOT" == "" ]; then
	QTROOT=~/Qt/5.5/clang_64
    fi

    QTDEPLOY=$QTROOT/bin/macdeployqt

    APPBUNDLE=iRODS.app
    APPBUNDLE_SRC=./src/$APPBUNDLE
    PKGROOT=./pkgroot

    PKG_PLIST=iRODS.app.pkg.plist
    PKG_VERSION=$VERSION
    PKG_ID=fi.jyu.iRODS.app
    PKG_INSTALL=/Applications
    PKG_FILE=iRODS.app-$VERSION.pkg

    # compile executable and build app bundle
    echo "compiling executable and building osx app bundle..."
    (cd src; $QTROOT/bin/qmake -spec macx-clang CONFIG+=x86_64 && make)

    # continue build only if make is successful
    if [ $? -ne "0" ]; then
      exit
    fi

    # setup package root with app bundle
    echo "staging package root..."
    mkdir -p $PKGROOT
    cp -R $APPBUNDLE_SRC $PKGROOT/

    # deploy Qt libraries
    echo "deploying Qt libaries to app bundle..."
    $QTDEPLOY $PKGROOT/$APPBUNDLE

    # build install package
    echo "building install package..."
    pkgbuild --root $PKGROOT \
	--component-plist $PKG_PLIST \
	--version $PKG_VERSION \
	--identifier $PKG_ID \
	--install-location $PKG_INSTALL \
	$PKG_FILE
else
    echo "Building for Linux:"
    
    # if no Qt path was provided, set to default
    if [ "$QTROOT" == "" ]; then
	QTROOT=/usr/lib64/qt5
    fi

    # build a makefile and make
    echo "building executable..."
    (cd src; $QTROOT/bin/qmake && make)

   # continue build only if make was successful
    if [ $? -ne "0" ]; then
        exit
    fi

    # create tar package
    echo "creating rpmbuild dist tarball package..."
    (cd src; tar cvzf ../kanki-irodsclient-$VERSION-linux-$OSRELEASE.tar.gz irodsclient schema.xml)
fi
