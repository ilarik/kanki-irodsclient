# kanki-irodsclient.pro
# Kanki irodsclient Qt project file
# (C) 2014-2016 University of Jyväskylä. All rights reserved.
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
    rodsstringconditionwidget.cpp \
    rodsconditionwidget.cpp \
    rodsdateconditionwidget.cpp \
    rodsmetadataconditionwidget.cpp

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
    rodsstringconditionwidget.h \
    rodsdateconditionwidget.h \
    rodsmetadataconditionwidget.h

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

    IRODS_PLUGINS_AUTH = libnative.so libosauth.so libpam.so
    IRODS_PLUGINS_AUTH_LIBS.path = Contents/PlugIns/irods/auth

    for (plugin, IRODS_PLUGINS_AUTH) {
        IRODS_PLUGINS_AUTH_LIBS.files += $$OSX_IRODS_BUILD/plugins/auth/$$plugin
    }

    IRODS_PLUGINS_NETWORK = libtcp.so libssl.so
    IRODS_PLUGINS_NETWORK_LIBS.path = Contents/PlugIns/irods/network

    for (plugin, IRODS_PLUGINS_NETWORK) {
        IRODS_PLUGINS_NETWORK_LIBS.files += $$OSX_IRODS_BUILD/plugins/network/$$plugin
    }

    IRODS_ICOMMANDS =   genOSAuth iadmin iapitest ibun icd ichksum ichmod icp idbug ienv ierror iexecmd \
                        iexit ifsck iget igetwild igroupadmin ihelp iinit ilocate ils ilsresc imcoll \
                        imeta imiscsvrinfo imkdir imv ipasswd iphybun iphymv ips iput ipwd iqdel iqmod \
                        iqstat iquest iquota ireg irepl irm irmtrash irods-grid irsync irule iscan isysmeta \
                        iticket itrim iuserinfo ixmsg izonereport
    IRODS_ICOMMANDS_BIN.path = Contents/PlugIns/irods/icommands

    for (icommand, IRODS_ICOMMANDS) {
        IRODS_ICOMMANDS_BIN.files += $$OSX_IRODS_BUILD/iRODS/clients/icommands/bin/$$icommand
    }

    for (plugin, IRODS_PLUGINS_AUTH) {
        target_auth_plugins.commands += install_name_tool -change \
        /usr/local/opt/openssl/lib/libssl.1.0.0.dylib \
        @executable_path/../../../Frameworks/libssl.1.0.0.dylib \
        $${TARGET}.app/$$IRODS_PLUGINS_AUTH_LIBS.path/$$plugin;
        target_auth_plugins.commands += install_name_tool -change \
        /usr/local/opt/openssl/lib/libcrypto.1.0.0.dylib \
        @executable_path/../../../Frameworks/libcrypto.1.0.0.dylib \
        $${TARGET}.app/$$IRODS_PLUGINS_AUTH_LIBS.path/$$plugin;
    }

    for (plugin, IRODS_PLUGINS_NETWORK) {
        target_network_plugins.commands += install_name_tool -change \
        /usr/local/opt/openssl/lib/libssl.1.0.0.dylib \
        @executable_path/../../../Frameworks/libssl.1.0.0.dylib \
        $${TARGET}.app/$$IRODS_PLUGINS_NETWORK_LIBS.path/$$plugin;
        target_network_plugins.commands += install_name_tool -change \
        /usr/local/opt/openssl/lib/libcrypto.1.0.0.dylib \
        @executable_path/../../../Frameworks/libcrypto.1.0.0.dylib \
        $${TARGET}.app/$$IRODS_PLUGINS_NETWORK_LIBS.path/$$plugin;
    }

    for (icommand, IRODS_ICOMMANDS) {
        target_icommands.commands += install_name_tool -change \
        /usr/local/opt/openssl/lib/libssl.1.0.0.dylib \
        @executable_path/../../../Frameworks/libssl.1.0.0.dylib \
        $${TARGET}.app/$$IRODS_ICOMMANDS_BIN.path/$$icommand;
        target_icommands.commands += install_name_tool -change \
        /usr/local/opt/openssl/lib/libcrypto.1.0.0.dylib \
        @executable_path/../../../Frameworks/libcrypto.1.0.0.dylib \
        $${TARGET}.app/$$IRODS_ICOMMANDS_BIN.path/$$icommand;
    }

    QMAKE_EXTRA_TARGETS += target_icommands #target_auth_plugins target_network_plugins
    PRE_TARGETDEPS += target_icommands #target_auth_plugins target_network_plugins

    QMAKE_BUNDLE_DATA += SCHEMA_XML
    QMAKE_BUNDLE_DATA += IRODS_PLUGINS_AUTH_LIBS
    QMAKE_BUNDLE_DATA += IRODS_PLUGINS_NETWORK_LIBS
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
   
macx {
    INCLUDEPATH += /usr/local/opt/openssl/include
    LIBS += -L/usr/local/opt/openssl/lib -lssl -lcrypto
}

macx {
    LIBS += -lc++
    LIBS += $$OSX_IRODS_BUILD/iRODS/lib/client_exec/obj/irods_client_rule_execution_manager_factory.o
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
    LIBS += -L/usr/lib/irods/externals -lirods_client_api -lirods_client -lboost_filesystem -lboost_regex -lboost_system -lboost_thread
    LIBS += -lboost_chrono -lboost_date_time -lboost_filesystem -lboost_iostreams -lboost_program_options
    LIBS += /usr/lib/irods/externals/libjansson.a
    LIBS += -lcrypto -lssl
}

LIBS += -ldl -lm -lpthread -lcurl

