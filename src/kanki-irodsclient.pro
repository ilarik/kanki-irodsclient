# kanki-irodsclient.pro
# Kanki irodsclient Qt project file
# (C) 2014-2015 University of Jyväskylä. All rights reserved.
# See LICENSE file for more information.

include(../config/build.pri)

QT       += core gui svg xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

macx {
    TARGET = irodsclient
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
    rodsobjlinkviz.cpp

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
    rodsobjlinkviz.h

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

    QMAKE_BUNDLE_DATA += SCHEMA_XML
}

macx {
    QMAKE_CXXFLAGS += -Dosx_platform #-DHIGH_DPI
}

else {
    QMAKE_CXXFLAGS += -std=c++0x
}

QMAKE_CXXFLAGS += -Wno-write-strings -fPIC -Wno-deprecated -D_FILE_OFFSET_BITS=64 -DPARA_OPR=1 -D_REENTRANT
QMAKE_CXXFLAGS += -DTAR_STRUCT_FILE -DGNU_TAR -DTAR_EXEC_PATH="/bin/tar" -DZIP_EXEC_PATH="/usr/bin/zip" -DUNZIP_EXEC_PATH="/usr/bin/unzip"
QMAKE_CXXFLAGS += -DPAM_AUTH -DUSE_BOOST

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

else {
    LIBS += -lssl -lcrypto
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
}
