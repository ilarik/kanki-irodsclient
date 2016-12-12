/**
 * @file rodsconnectthread.h
 * @brief Definition of class RodsConnectThread
 *
 * The RodsConnectThread class extends the Qt thread management class
 * QThread and implements a worker thread for establishing a
 * connection to an iRODS server.
 *
 * Copyright (C) 2016 KTH Royal Institute of Technology. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * Copyright (C) 2014-2016 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

#ifndef RODSCONNECTTHREAD_H
#define RODSCONNECTTHREAD_H

// Qt framework headers
#include <QThread>
#include <QString>

// Kanki iRODS C++ class library headers
#include "rodsconnection.h"

class RodsConnectThread : public QThread
{
    Q_OBJECT

signals:

    // Qt signal for initializing progress bar display, signals out
    // the initial text, initial value and max value of progress bar.
    void startProgressDisplay(QString text, int value, int maxValue);

    // Qt signal for updating the progress bar display, signals out
    // current text message and current progress value.
    void progressUpdate(QString text, int progress);

    // Qt signal for signaling out a successful connection process.
    void success();

    // Qt signal for signaling out a failed connection process.
    void failure();

    // Qt signal for signaling out a connection object pointer for
    // a successful iRODS connection.
    void setConnection(Kanki::RodsConnection *conn);

    // Qt signal for reporting errors in the connection process,
    // signals out strings for a message, error message and error code.
    void reportError(QString msgStr, QString errorStr, int errorCode);

private:

    // Overrides superclass virtual function, executes connect process
    // in a worker thread instantiated with the thread object.
    void run() Q_DECL_OVERRIDE;
};

#endif // RODSCONNECTTHREAD_H
