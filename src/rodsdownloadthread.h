/**
 * @file rodsdownloadthread.h
 * @brief Definition of class RodsDownloadThread
 *
 * The RodsDownloadThread class extends the Qt thread management class
 * QThread and implements a worker thread for a download (get) operation.
 *
 * Copyright (C) 2016 KTH Royal Institute of Technology. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * Copyright (C) 2014-2016 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

#ifndef RODSDOWNLOADTHREAD_H
#define RODSDOWNLOADTHREAD_H

// C++ standard library headers
#include <chrono>

// boost library headers
#include <boost/thread/thread.hpp>
#include <boost/filesystem.hpp>

// Qt framework headers
#include <QThread>
#include <QString>
#include <QStringList>
#include <QProgressDialog>
#include <QVariant>
#include <QDir>
#include <QFile>

// new-age iRODS headers
#include "dstream.hpp"
#include "transport/default_transport.hpp"
#include "thread_pool.hpp"
#include "connection_pool.hpp"

// Kanki iRODS C++ class library headers
#include "rodsconnection.h"
#include "rodsobjentry.h"
#include "rodsdatainstream.h"

class RodsDownloadThread : public QThread
{
    Q_OBJECT

public:

    // Constructor initializes the download worker thread and sets its parameters for execution.
    RodsDownloadThread(Kanki::RodsConnection *theConn, Kanki::RodsObjEntryPtr theObj, const std::string &theDestPath,
                       bool verifyChecksum = true, bool allowOverwrite = false);

signals:

    // Qt signal for initializing a progress bar display, it signals out
    // the initial message text, initial value and maximum value.
    void setupMainProgress(QString text, int value, int maxValue);

    // Qt signal for updating the progress bar display, it signals out
    // the current message text and current progress value.
    void mainProgressUpdate(QString text, int progress);

    // Qt signal for setting the progress bar display in marquee mode,
    // it signals out the current text message.
    void mainProgressMarquee(QString text);

    // Qt signal for initializing the progress bar display subprogress
    // display, signals out initial message, initial value and max value
    void setupSubProgress(QString itemName, QString text, int value, int maxValue);

    // Qt signal for updating the secondary progress bar display,
    // it signals out the current secondary status msg and progress value
    void subProgressUpdate(QString itemName, QString text, int progress);

    // Qt signal for setting a sub progress bar display in marquee mode,
    // it gets the sub progress identifier and a string to display
    void subProgressMarquee(QString itemName, QString text); 

    // Qt signal to finalize a sub progress bar display,
    // it receives only the sub progress identifier
    void subProgressFinalize(QString itemName);

    // Qt signal for reporting errors to ui, it signals out a message,
    // an error string and an error code.
    void reportError(QString msgStr, QString errorStr, int errorCode);

private:

    // Overrides superclass virtual function, executes the download
    // work in a thread instantiated with the thread object.
    void run() Q_DECL_OVERRIDE;

    // Constructs the list of objects to be downloaded in a recursive manner.
    int makeCollObjList(Kanki::RodsObjEntryPtr obj, std::vector<Kanki::RodsObjEntryPtr> *objs);

    // Get (download) one rods data object, launching parallel transfer if necessary
    int getObject(irods::connection_pool::connection_proxy &conn,
		  Kanki::RodsObjEntryPtr obj, std::string localPath,
		  bool verifyChecksum = false, bool allowOverwrite = true);

    // Implements double-buffered rods object download using Kanki::RodsDataInStream
    // and its adaptive rods i/o request size scaling for best resposniveness and
    // connection throughput utilization.
    int transferFileStream(Kanki::RodsObjEntryPtr obj, std::istream &inStream, std::ofstream &outStream);

    // Launches parallel transfer for maximum performance and direct I/O to rods resource servers
    int transferFileParallel(Kanki::RodsObjEntryPtr obj, std::istream &inStream, std::ofstream &outStream);

    // pointer to the rods connection object
    Kanki::RodsConnection *conn;

    // pointer to the rods object entry to be downloaded
    Kanki::RodsObjEntryPtr objEntry;

    // destination path (local directory) for the download
    std::string destPath;

    // settings for the download operation, verify checksum and allow overwrite
    bool verify, overwrite;
};

#endif // RODSDOWNLOADTHREAD_H
