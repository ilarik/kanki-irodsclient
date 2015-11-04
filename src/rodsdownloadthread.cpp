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

RodsDownloadThread::RodsDownloadThread(Kanki::RodsConnection *theConn, Kanki::RodsObjEntryPtr theObj,
                                       const std::string &theDestPath, bool verifyChecksum, bool allowOverwrite)
    : QThread()
{
    this->conn = new Kanki::RodsConnection(theConn);
    this->objEntry = theObj;
    this->destPath = theDestPath;

    this->verify = verifyChecksum;
    this->overwrite = allowOverwrite;
}

void RodsDownloadThread::run()
{
    int status = 0;
    QString statusStr = "Initializing...";

    // signal ui to setup progress display
    progressMarquee(statusStr);

    // open the parallel connection for transfer and authenticate
    if ((status = this->conn->connect()) < 0)
    {
        reportError("Download failed", "Open parallel connection failed", status);
        return;
    }

    else if ((status = this->conn->login()) < 0)
    {
        reportError("Download failed", "Authentication failed", status);
        return;
    }

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
                    statusStr = "Downloading ";
                    statusStr += curObj->getObjectName().c_str();
                    progressUpdate(statusStr, i+1);

                    // try to do a rods get operation
                    if ((status = this->downloadFile(curObj, dstPath, this->verify, this->overwrite)) < 0)
                        reportError("iRODS get file error", curObj->getObjectFullPath().c_str(), status);
                }

                // for collection objects we create the corresponding directory
                else if (curObj->objType == COLL_OBJ_T)
                {
                    // get directory name for ui
                    std::string dirName = dstPath.substr(dstPath.find_last_of('/') + 1);

                    // notify ui
                    statusStr = "Creating directory ";
                    statusStr += dirName.c_str();
                    progressUpdate(statusStr, i+1);

                    // check if directory exists and if not, make it
                    QDir dstDir(dstPath.c_str());

                    if (!dstDir.exists())
                        dstDir.mkpath(dstPath.c_str());
                }
            }
        }
    }

    // in the case of downloading a single data object, a simple get operation
    else if (this->objEntry->objType == DATA_OBJ_T)
    {
        QString statusStr = "Downloading file: ";

        statusStr += this->objEntry->getObjectName().c_str();
        std::string dstPath = this->destPath + "/" + this->objEntry->getObjectName();
        setupProgressDisplay(statusStr, 1, 1);

        // try to do a rods get operation
        if ((status = this->downloadFile(objEntry, dstPath, this->verify, this->overwrite)) < 0)
            reportError("Download failed", "Kanki data stream error", status);
    }

    this->conn->disconnect();
    delete(this->conn);
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

int RodsDownloadThread::downloadFile(Kanki::RodsObjEntryPtr obj, std::string localPath,
                                     bool verifyChecksum, bool allowOverwrite)
{
    Kanki::RodsDataInStream inStream(this->conn, obj);
    long int status = 0, lastRead = 0, totalRead = 0;
    QFile localFile(localPath.c_str());
    long int readSize = __KANKI_BUFSIZE_MAX;
    void *buffer = std::malloc(readSize), *buffer2 = std::malloc(readSize);
    boost::thread *writer = NULL;

    // check if we're allowed to proceed
    if (localFile.exists() && !allowOverwrite)
        return (OVERWRITE_WITHOUT_FORCE_FLAG);

    // try to open local file and the rods data stream
    if (!localFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return (-1);

    // try to initiate get operation and open data stream
    if ((status = inStream.getOprInit()) < 0)
        return (status);

    if ((status = inStream.openDataObj()) < 0)
        return (status);

    else {
        // update status display only on large enough objects
        if (obj->objSize > __KANKI_BUFSIZE_INIT)
            setupSubProgressDisplay("Transferring...", 0, 100);

        std::chrono::high_resolution_clock::time_point t0 = std::chrono::high_resolution_clock::now();

        while ((lastRead = inStream.readAdaptive(buffer, readSize)) > 0)
        {
            std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
            std::chrono::milliseconds diff = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);

            totalRead += lastRead;

            // if we had something to write, wait for it
            if (writer)
            {
                writer->join();
                delete (writer);

                // check for write errors
                if (localFile.error() == QFile::WriteError)
                {
                    reportError("Download failed", "Write error", -1);
                    verifyChecksum = false;

                    break;
                }
            }

            // (re)new writer thread
            writer = new boost::thread(boost::bind(&QFile::write, &localFile, (const char*)buffer, lastRead));

            // XOR swap buffer pointers
            buffer = (void*)((uintptr_t)buffer ^ (uintptr_t)buffer2);
            buffer2 = (void*)((uintptr_t)buffer ^ (uintptr_t)buffer2);
            buffer = (void*)((uintptr_t)buffer ^ (uintptr_t)buffer2);

            // compute and signal statistics to UI
            double speed = ((double)totalRead / 1048576) / ((double)diff.count() / 1000);
            double percentage = ceil(((double)totalRead / (double)obj->objSize) * 100);

            QString statusStr = "Transferring... " + QVariant((int)percentage).toString() + "%";
            statusStr += " at " + QString::number(speed, 'f', 2) + " MB/s";

            if (obj->objSize > __KANKI_BUFSIZE_INIT)
                subProgressUpdate(statusStr, (int)percentage);
        }
    }

    // close local file and rods data stream
    localFile.close();
    status = inStream.closeDataObj();
    inStream.getOprEnd();

    // if verify checksum was required
    if (verifyChecksum && strlen(inStream.checksum()))
    {
        subProgressUpdate("Verifying Checksum...", 100);
        status = verifyChksumLocFile((char*)localPath.c_str(), (char*)inStream.checksum(), NULL);
    }

    // free everything we have allocated
    if (writer)
        delete (writer);

    std::free(buffer);
    std::free(buffer2);

    return (status);
}
