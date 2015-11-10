# kanki-irodsclient.pro
# Kanki irodsclient Qt project file
# (C) 2014-2015 University of Jyväskylä. All rights reserved.
# See LICENSE file for more information.

include(../config/build.pri)

QT       += core gui svg xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

macx {
    TARGET = iRODS
}

else {
    TARGET = irodsclient
}

macx {
    TEMPLATE = app
}

SOURCES += main.cpp\
        rodsmainwindow.cpp \
    rodsconnection.cpp \
    rodsobjtreeitem.cpp \
    rodsobjtreemodel.cpp \
    rodsgenquery.cpp \
    rodsmetadatawindow.cpp \
    rodsqueuewindow.cpp \
    rodsqueuemodel.cpp \
    rodsmetadataitem.cpp \
    rodsmetadatamodel.cpp \
    rodsobjmetadata.cpp \
    rodsfindwindow.cpp \
    rodsmetadataschema.cpp \
    rodsconnectthread.cpp \
    rodsdownloadthread.cpp \
    rodsobjentry.cpp \
    rodsuploadthread.cpp \
    rodstransferwindow.cpp \
    rodsdatastream.cpp \
    rodsdatainstream.cpp \
    rodsdataoutstream.cpp \
    rodserrorlogwindow.cpp \
    rodscriteriawidget.cpp \
    rodsstringconditionwidget.cpp

HEADERS  += rodsmainwindow.h \
    rodsconnection.h \
    rodsobjtreeitem.h \
    rodsobjtreemodel.h \
    rodsmetadatawindow.h \
    rodsqueuewindow.h \
    rodsqueuemodel.h \
    rodsmetadataitem.h \
    rodsmetadatamodel.h \
    rodsobjmetadata.h \
    rodsfindwindow.h \
    rodsmetadataschema.h \
    rodsconnectthread.h \
    rodsdownloadthread.h \
    rodsobjentry.h \
    version.h \
    rodsuploadthread.h \
    rodstransferwindow.h \
    rodsdatastream.h \
    rodsdatainstream.h \
    rodsdataoutstream.h \
    _rodsgenquery.h \
    rodserrorlogwindow.h \
    rodsconditionwidget.h \
    rodsstringconditionwidget.h

FORMS    += rodsmainwindow.ui \
    rodsmetadatawindow.ui \
    rodsqueuewindow.ui \
    rodsfindwindow.ui

RESOURCES += \
    icons.qrc

OTHER_FILES += \
    irods.icns \
    AppConfig.plist \
    server.crt \
    build.sh \
    iRODS.app.pkg.plist \
    schema.xml \
    LICENSE

macx {
    ICON = irods.icns
}

macx {
    SCHEMA_XML.files = schema.xml
    SCHEMA_XML.path = Contents/Resources

    IRODS_PLUGINS_AUTH.files = \
        $$OSX_IRODS_BUILD/plugins/auth/libnative.so \
        $$OSX_IRODS_BUILD/plugins/auth/libosauth.so \
        $$OSX_IRODS_BUILD/plugins/auth/libpam.so
    IRODS_PLUGINS_AUTH.path = Contents/PlugIns/irods/auth

    IRODS_PLUGINS_NETWORK.files = \
        $$OSX_IRODS_BUILD/plugins/network/libtcp.so \
        $$OSX_IRODS_BUILD/plugins/network/libssl.so
    IRODS_PLUGINS_NETWORK.path = Contents/PlugIns/irods/network

    IRODS_ICOMMANDS_BIN.files = \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/genOSAuth \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/iadmin \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/iapitest \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/ibun \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/icd \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/ichksum \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/ichmod \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/icp \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/idbug \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/ienv \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/ierror \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/iexecmd \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/iexit \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/ifsck \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/iget \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/igetwild \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/igroupadmin \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/ihelp \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/iinit \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/ilocate \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/ils \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/ilsresc \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/imcoll \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/imeta \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/imiscsvrinfo \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/imkdir \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/imv \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/ipasswd \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/iphybun \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/iphymv \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/ips \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/iput \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/ipwd \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/iqdel \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/iqmod \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/iqstat \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/iquest \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/iquota \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/ireg \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/irepl \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/irm \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/irmtrash \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/irods-grid \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/irsync \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/irule \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/iscan \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/isysmeta \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/iticket \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/itrim \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/iuserinfo \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/ixmsg \
        $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/izonereport
    IRODS_ICOMMANDS_BIN.path = Contents/PlugIns/irods/icommands

    QMAKE_BUNDLE_DATA += SCHEMA_XML
    QMAKE_BUNDLE_DATA += IRODS_PLUGINS_AUTH
    QMAKE_BUNDLE_DATA += IRODS_PLUGINS_NETWORK
    QMAKE_BUNDLE_DATA += IRODS_ICOMMANDS_BIN
}

macx {
    QMAKE_CXXFLAGS += -Dosx_platform #-DHIGH_DPI
}

else {
    QMAKE_CXXFLAGS += -std=c++0x
}

QMAKE_CXXFLAGS += -Wno-write-strings -fPIC -Wno-deprecated -D_FILE_OFFSET_BITS=64 -DPARA_OPR=1 -D_REENTRANT
QMAKE_CXXFLAGS += -DTAR_STRUCT_FILE -DGNU_TAR -DTAR_EXEC_PATH="/bin/tar" -DZIP_EXEC_PATH="/usr/bin/zip" -DUNZIP_EXEC_PATH="/usr/bin/unzip"
QMAKE_CXXFLAGS += -DPAM_AUTH -DUSE_BOOST -DBUILD_HOST=\\\"`hostname`\\\" -DBUILD_TAG=\\\"$(BUILD_TAG)\\\"

macx {
    QMAKE_INFO_PLIST = AppConfig.plist
}

INCLUDEPATH += /usr/include/openssl

macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.10

    INCLUDEPATH += $$OSX_IRODS_BUILD/external/$$OSX_IRODS_BOOST
    INCLUDEPATH += $$OSX_IRODS_BUILD/iRODS/lib/core/include
    INCLUDEPATH += $$OSX_IRODS_BUILD/iRODS/lib/api/include
    INCLUDEPATH += $$OSX_IRODS_BUILD/iRODS/lib/hasher/include
    INCLUDEPATH += $$OSX_IRODS_BUILD/iRODS/server/core/include
    INCLUDEPATH += $$OSX_IRODS_BUILD/iRODS/server/icat/include
    INCLUDEPATH += $$OSX_IRODS_BUILD/iRODS/server/drivers/include
    INCLUDEPATH += $$OSX_IRODS_BUILD/iRODS/server/re/include
    INCLUDEPATH += $$OSX_IRODS_BUILD/iRODS/lib/api/include
    INCLUDEPATH += $$OSX_IRODS_BUILD/iRODS/lib/md5/include
    INCLUDEPATH += $$OSX_IRODS_BUILD/iRODS/lib/sha1/include
    INCLUDEPATH += $$OSX_IRODS_BUILD/iRODS/lib/rbudp/include
}

else {
    INCLUDEPATH += /usr/include/irods
    INCLUDEPATH += /usr/include/irods/boost
}
   
LIBS += -ldl -lm -lpthread -lcurl

macx {
    INCLUDEPATH += /usr/local/opt/openssl/include
    LIBS += -L/usr/local/opt/openssl/lib -lssl -lcrypto
}

macx {
    LIBS += -lc++
    LIBS += $$OSX_IRODS_BUILD/iRODS/lib/core/obj/libRodsAPIs.a
    LIBS += $$OSX_IRODS_BUILD/external/$$OSX_IRODS_BOOST/stage/lib/libboost_filesystem.a
    LIBS += $$OSX_IRODS_BUILD/external/$$OSX_IRODS_BOOST/stage/lib/libboost_regex.a
    LIBS += $$OSX_IRODS_BUILD/external/$$OSX_IRODS_BOOST/stage/lib/libboost_system.a
    LIBS += $$OSX_IRODS_BUILD/external/$$OSX_IRODS_BOOST/stage/lib/libboost_thread.a
    LIBS += $$OSX_IRODS_BUILD/external/$$OSX_IRODS_BOOST/stage/lib/libboost_chrono.a
    LIBS += $$OSX_IRODS_BUILD/external/$$OSX_IRODS_BOOST/stage/lib/libboost_date_time.a
    LIBS += $$OSX_IRODS_BUILD/external/$$OSX_IRODS_BOOST/stage/lib/libboost_iostreams.a
    LIBS += $$OSX_IRODS_BUILD/external/$$OSX_IRODS_BOOST/stage/lib/libboost_program_options.a
    LIBS += $$OSX_IRODS_BUILD/external/$$OSX_IRODS_JANSSON/src/.libs/libjansson.a
} 

else {
    LIBS += -L/usr/lib/irods/externals -lirods_client -lirods_client_api -lboost_filesystem -lboost_regex -lboost_system -lboost_thread 
    LIBS += -lboost_chrono -lboost_date_time -lboost_filesystem -lboost_iostreams -lboost_program_options
    LIBS += /usr/lib/irods/externals/libjansson.a
    LIBS += -lcrypto -lssl
}
