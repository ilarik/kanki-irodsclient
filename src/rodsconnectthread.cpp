/**
 * @file rodsconnectthread.cpp
 * @brief Implementation of class RodsConnectThread
 *
 * The RodsConnectThread class extends the Qt thread management class
 * QThread and implements a worker thread for establishing a
 * connection to an iRODS server.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

// application class RodsConnectThread header
#include "rodsconnectthread.h"

void RodsConnectThread::run()
{
    Kanki::RodsConnection *newConn = NULL;
    int status = 0;

    progressUpdate("Connecting to iRODS...", 1);
    newConn = new Kanki::RodsConnection();

    // try to connect to the iRODS server
    if ((status = newConn->connect()) < 0)
    {
        reportError("iRODS connection error", newConn->lastErrorMsg().c_str(),
                    newConn->lastError());

        delete(newConn);

        // signal connection attempt failure
        setConnection(NULL);
        failure();
    }

    // second phase - user authentication
    else {
        progressUpdate("Authenticating...", 2);

        // try to authenticate while reporting error trough ui
        if ((status = newConn->login()) < 0)
        {
            reportError("iRODS login error, try using 'iinit' command", newConn->lastErrorMsg().c_str(),
                        newConn->lastError());

            delete(newConn);

            setConnection(NULL);
            failure();
        }

        // on success, signal out the newly created connection object
        else {
            setConnection(newConn);
            success();
        }
    }
}
