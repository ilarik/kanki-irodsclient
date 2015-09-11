/**
 * @file rodsdownloadthread.h
 * @brief Definition of class RodsDownloadThread
 *
 * The RodsDownloadThread class extends the Qt thread management class
 * QThread and implements a worker thread for a download (get) operation.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

#ifndef RODSDOWNLOADTHREAD_H
#define RODSDOWNLOADTHREAD_H

// Qt framework headers
#include <QThread>
#include <QString>
#include <QStringList>
#include <QProgressDialog>
#include <QVariant>
#include <QDir>

// Kanki iRODS C++ class library headers
#include "rodsconnection.h"
#include "rodsobjentry.h"

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
    void setupProgressDisplay(QString text, int value, int maxValue);

    // Qt signal for updating the progress bar display, it signals out
    // the current message text and current progress value.
    void progressUpdate(QString text, int progress);

    // Qt signal for setting the progress bar display in marquee mode,
    // it signals out the current text message.
    void progressMarquee(QString text);

    // Qt signal for signaling out that download operation is complete.
    void done();

    // Qt signal for reporting errors to ui, it signals out a message,
    // an error string and an error code.
    void reportError(QString msgStr, QString errorStr, int errorCode);

private:

    // Overrides superclass virtual function, executes the download
    // work in a thread instantiated with the thread object.
    void run() Q_DECL_OVERRIDE;

    // Constructs the list of objects to be downloaded in a recursive manner.
    int makeCollObjList(Kanki::RodsObjEntryPtr obj, std::vector<Kanki::RodsObjEntryPtr> *objs);

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
