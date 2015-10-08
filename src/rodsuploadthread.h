/**
 * @file rodsuploadthread.h
 * @brief Definition of class RodsUploadThread
 *
 * The RodsUploadThread class extends the Qt thread management class QThread
 * and implements a worker thread for an iRODS put operation.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

#ifndef RODSUPLODADTHREAD_H
#define RODSUPLODADTHREAD_H

// Qt framework headers
#include <QThread>
#include <QString>
#include <QStringList>
#include <QProgressDialog>
#include <QFileInfo>
#include <QDir>

// Kanki iRODS C++ class library headers
#include "rodsconnection.h"

// application headers
#include "rodsmainwindow.h"
#include "rodsobjtreemodel.h"

class RodsUploadThread : public QThread
{
    Q_OBJECT

public:

    // Constructor initializes the upload worker thread and sets its parameters for execution,
    // requires a rods conn pointer, file paths list and dest coll path.
    RodsUploadThread(Kanki::RodsConnection *theConn, QStringList filePaths,
                     std::string destColl, std::string rodsResc);

    // Constructor initializes the upload worker thread and sets its parameters for execution,
    // requires a rods conn pointer, base path for recursive upload and dest coll path.
    RodsUploadThread(Kanki::RodsConnection *theConn, std::string baseDirPath,
                     std::string destColl, std::string rodsResc);

signals:

    // Qt signal for initializing a progress bar display, it signals out
    // the initial message text, initial value and maximum value.
    void setupProgressDisplay(QString text,int value, int maxValue);

    // Qt signal for updating the progress bar display, it signals out
    // the current message text and current progress value.
    void progressUpdate(QString text, int progress);

    // Qt signal for setting the progress bar display in marquee mode,
    // it signals out the current text message.
    void progressMarquee(QString text);

    // Qt signal for reporting errors to ui, it signals out a message,
    // an error string and an error code.
    void reportError(QString msgStr, QString errorStr, int errorCode);

    // Qt signal for requesing a rods object tree model refresh,
    // at a given index (after upload complete, signals ui).
    void refreshObjectModel(QString path);

private:

    // Overrides superclass virtual function, executes the upload
    // work in a thread instantiated with the thread object.
    void run() Q_DECL_OVERRIDE;

    // Makes a bill of materials list for the upload, i.e recursively reads
    // the local directory for files and directories paths.
    void makeBillOfMaterials(const QString &dirPath, QStringList *filePaths);

    // pointer to the rods connection object
    Kanki::RodsConnection *conn;

    // bill of materials for the upload
    QStringList filePathList;

    // destination rods collection path
    std::string destCollPath, basePath, targetResc;
};
#endif // RODSUPLODADTHREAD_H
