# kanki-irodsclient.pro
# Kanki irodsclient Qt project file
# (C) 2014-2015 University of Jyväskylä. All rights reserved.
# See LICENSE file for more information.

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
    rodsuploadthread.cpp

HEADERS  += rodsmainwindow.h \
    rodsconnection.h \
    rodsobjtreeitem.h \
    rodsobjtreemodel.h \
    rodsgenquery.h \
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
    rodsuploadthread.h

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
    QMAKE_CXXFLAGS += -Dosx_platform 
}

QMAKE_CXXFLAGS += -Wno-write-strings -fPIC -Wno-deprecated -D_FILE_OFFSET_BITS=64 -DPARA_OPR=1 -D_REENTRANT
QMAKE_CXXFLAGS += -DTAR_STRUCT_FILE -DGNU_TAR -DTAR_EXEC_PATH="/bin/tar" -DZIP_EXEC_PATH="/usr/bin/zip" -DUNZIP_EXEC_PATH="/usr/bin/unzip"
QMAKE_CXXFLAGS += -DPAM_AUTH -DUSE_BOOST #-DHIGH_DPI

macx {
    QMAKE_INFO_PLIST = AppConfig.plist
}

INCLUDEPATH += /usr/include/openssl

macx {
    INCLUDEPATH += /var/lib/irods/build/external/boost_1_55_0z
    INCLUDEPATH += /var/lib/irods/build/iRODS/lib/core/include
    INCLUDEPATH += /var/lib/irods/build/iRODS/server/core/include
    INCLUDEPATH += /var/lib/irods/build/iRODS/server/icat/include
    INCLUDEPATH += /var/lib/irods/build/iRODS/server/drivers/include
    INCLUDEPATH += /var/lib/irods/build/iRODS/server/re/include
    INCLUDEPATH += /var/lib/irods/build/iRODS/lib/api/include
    INCLUDEPATH += /var/lib/irods/build/iRODS/lib/md5/include
    INCLUDEPATH += /var/lib/irods/build/iRODS/lib/sha1/include
    INCLUDEPATH += /var/lib/irods/build/iRODS/lib/rbudp/include
}

else {
    INCLUDEPATH += /usr/include/irods
    INCLUDEPATH += /usr/include/irods/boost
}
   
LIBS += -ldl -lm -lpthread -lcurl -lssl -lcrypto

macx {
    LIBS += /var/lib/irods/build/iRODS/lib/core/obj/libRodsAPIs.a
    LIBS += /var/lib/irods/build/external/boost_1_55_0z/stage/lib/libboost_filesystem.a
    LIBS += /var/lib/irods/build/external/boost_1_55_0z/stage/lib/libboost_regex.a
    LIBS += /var/lib/irods/build/external/boost_1_55_0z/stage/lib/libboost_system.a
    LIBS += /var/lib/irods/build/external/boost_1_55_0z/stage/lib/libboost_thread.a
} 

else {
    LIBS += -L/usr/lib/irods/externals -lirods_client -lirods_client_api -lboost_filesystem -lboost_regex -lboost_system -lboost_thread 
    LIBS += -lboost_chrono -lboost_date_time -lboost_filesystem -lboost_iostreams -lboost_program_options
    LIBS += /usr/lib/irods/externals/libjansson.a
}
