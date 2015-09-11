/**
 * @file rodsdownloadthread.h
 * @brief Implementation of class RodsDownloadThread
 *
 * The RodsDownloadThread class extends the Qt thread management class
 * QThread and implements a worker thread for a download (get) operation.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

// application class RodsDownloadThread header
#include "rodsdownloadthread.h"

RodsDownloadThread::RodsDownloadThread(Kanki::RodsConnection *theConn, Kanki::RodsObjEntryPtr theObj, const std::string &theDestPath,
                                       bool verifyChecksum, bool allowOverwrite)
    : QThread()
{
    this->conn = theConn;
    this->objEntry = theObj;
    this->destPath = theDestPath;

    this->verify = verifyChecksum;
    this->overwrite = allowOverwrite;
}

void RodsDownloadThread::run() Q_DECL_OVERRIDE
{
    int status = 0;
    QString statusStr = "Initializing...";

    // signal ui to setup progress display
    setupProgressDisplay(statusStr, 0, 1);
    progressMarquee(statusStr);

    // in the case of downloading a collection, do it recursively
    if (this->objEntry->objType == COLL_OBJ_T)
    {
        std::vector<Kanki::RodsObjEntryPtr> collObjs;
        std::string basePath = this->objEntry->collPath;

        // first item to the download object list
        collObjs.push_back(this->objEntry);

        // try to construct the download object list recursively
        if (!(status = makeCollObjList(this->objEntry, &collObjs)))
        {
            // notify ui of progress bar state (object count)
            setupProgressDisplay(statusStr, 0, collObjs.size());

            // iterate thru object list
            for (unsigned int i = 0; i < collObjs.size(); i++)
            {
                Kanki::RodsObjEntryPtr curObj = collObjs.at(i);

                std::string basePath = this->objEntry->getObjectBasePath();
                std::string objPath = curObj->getObjectFullPath();

                objPath.erase(objPath.begin(), objPath.begin() + basePath.size());
                std::string dstPath = this->destPath + objPath;

                // in the case of a data object, we do a get operation
                if (curObj->objType == DATA_OBJ_T)
                {
                    // notify ui of current operation and progress
                    statusStr = "Downloading file ";
                    statusStr += curObj->getObjectName().c_str();
                    progressUpdate(statusStr, i);

                    // try to do a rods get operation
                    if ((status = this->conn->getFile(dstPath, curObj->getObjectFullPath(), this->verify, this->overwrite)) < 0)
                        reportError("iRODS get file error", "Get failed", status);
                }

                // for collection objects we create the corresponding directory
                else if (curObj->objType == COLL_OBJ_T)
                {
                    // notify ui
                    statusStr = "Creating directory ";
                    statusStr += dstPath.c_str();
                    progressUpdate(statusStr, i);

                    // check if directory exists and if not, make it
                    QDir dstDir(dstPath.c_str());

                    if (!dstDir.exists())
                    {
                        dstDir.mkpath(dstPath.c_str());
                    }
                }
            }
        }
    }

    // in the case of downloading a single data object, a simple get operation
    else if (this->objEntry->objType == DATA_OBJ_T)
    {
        QString statusStr = "Downloading file: ";

        statusStr += this->objEntry->getObjectName().c_str();
        progressMarquee(statusStr);

        std::string dstPath = this->destPath + "/" + this->objEntry->getObjectName();

        // try to do a rods get operation
        if ((status = this->conn->getFile(dstPath, this->objEntry->getObjectFullPath(), this->verify, this->overwrite)) < 0)
            reportError("iRODS get file error", "Get failed", status);
    }

    done();
}

int RodsDownloadThread::makeCollObjList(Kanki::RodsObjEntryPtr obj, std::vector<Kanki::RodsObjEntryPtr> *objs)
{
    int status = 0;

    // we proceed only for collections
    if (obj->objType == COLL_OBJ_T)
    {
        std::vector<Kanki::RodsObjEntryPtr> curCollObjs;

        // try to read collection
        if ((status = this->conn->readColl(obj->collPath, &curCollObjs)) >= 0)
        {
            // iterate thru current collection
            for (std::vector<Kanki::RodsObjEntryPtr>::iterator i = curCollObjs.begin(); i != curCollObjs.end(); i++)
            {
                // add object to list
                Kanki::RodsObjEntryPtr curObj = *i;
                objs->push_back(curObj);

                // notify ui
                QString statusStr = "Building a list of objects (" + QVariant((int)objs->size()).toString() + ")...";
                progressMarquee(statusStr);

                // recurse on collection objects
                if (curObj->objType == COLL_OBJ_T)
                {
                    status = makeCollObjList(curObj, objs);

                    // on error, back off recursion
                    if (status < 0)
                        return (status);
                }
            }
        }
    }

    return (status);
}
