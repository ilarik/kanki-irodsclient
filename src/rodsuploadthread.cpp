/**
 * @file rodsuploadthread.cpp
 * @brief Implementation of class RodsUploadThread
 *
 * The RodsUploadThread class extends the Qt thread management class QThread
 * and implements a worker thread for an iRODS put operation.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

// application class RodsUploadThread header
#include "rodsuploadthread.h"

RodsUploadThread::RodsUploadThread(Kanki::RodsConnection *theConn, QStringList filePaths,
                                   std::string destColl, std::string rodsResc)
    : QThread()
{
    this->conn = new Kanki::RodsConnection(theConn);

    this->filePathList = filePaths;
    this->destCollPath = destColl;
    this->targetResc = rodsResc;
}

RodsUploadThread::RodsUploadThread(Kanki::RodsConnection *theConn, std::string baseDirPath,
                                   std::string destColl, std::string rodsResc)
    : QThread()
{
    this->conn = new Kanki::RodsConnection(theConn);

    this->basePath = baseDirPath;
    this->destCollPath = destColl;
    this->targetResc = rodsResc;
}

void RodsUploadThread::run()
{
    QString statusStr = "Initializing...";
    int status = 0;
    int c = 1;

    // signal ui to setup progress display
    progressMarquee(statusStr);

    // open a parallel connection for the transfer and authenticate
    if ((status = this->conn->connect()) < 0)
    {
        reportError("Upload failed", "Open parallel connection failed", status);
        return;
    }

    else if ((status = this->conn->login()) < 0)
    {
        reportError("Upload failed", "Authentication failed", status);
        return;
    }

    // if we have no path list, we are uploading from a base path
    if (!this->filePathList.size())
    {
        // construct path list
        this->makeBillOfMaterials(this->basePath.c_str(), &this->filePathList);

        // make dest collection
        std::string destColl = this->destCollPath + this->basePath.substr(this->basePath.find_last_of('/'));
        if ((status = this->conn->makeColl(destColl, false)) < 0)
        {
            reportError("Upload failed!", "iRODS make collection failed", status);
            return;
        }
    }

    // signal ui to reset progress display
    setupProgressDisplay(statusStr, 0, this->filePathList.size());

    // iterate trough Qt string list containing the file names
    for (QStringList::const_iterator i = this->filePathList.begin(); i != this->filePathList.end(); i++)
    {
        // get next file path to upload
        QString fileName = *i;

        // get local path and file name
        std::string path = fileName.toStdString();
        std::string name = path.substr(path.find_last_of('/') + 1);
        std::string objPath = this->destCollPath + "/";

        if (this->basePath.empty())
            objPath += name;

        else {
            path.erase(path.begin(), path.begin() + this->basePath.size());
            objPath += this->basePath.substr(this->basePath.find_last_of('/') + 1) + path;
        }

        // check whether current path is a directory
        QFileInfo fileInfo(fileName);

        // the file is a directory, make rods collection
        if (fileInfo.isDir())
        {          
            // notify ui
            statusStr = "Creating collection '";
            statusStr += name.c_str();
            progressUpdate(statusStr, c);

            if ((status = this->conn->makeColl(objPath, false)) < 0)
                reportError("iRODS make collection error", "Put failed", status);
        }

        // if the file is a regular file with read permissions, upload
        if (fileInfo.isFile() && fileInfo.isReadable())
        {
            std::string statusStr = "Uploading file " + name;
            progressUpdate(statusStr.c_str(), c);

            // try to put file and report possible error to user
            if ((status = this->conn->putFile(fileName.toStdString(), objPath, this->targetResc)) < 0)
                reportError("iRODS put file error", objPath.c_str(), status);
        }

        c++;
    }

    // cleanly disconnect parallel connection
    this->conn->disconnect();
    delete(this->conn);

    // signal out a request for ui to refresh itself
    refreshObjectModel(QString(this->destCollPath.c_str()));
}

void RodsUploadThread::makeBillOfMaterials(const QString &dirPath, QStringList *filePaths)
{
    QString statusStr = "Initializing...";

    // signal ui to setup progress display
    progressMarquee(statusStr);

    // file info for sanity check
    QFileInfo fileInfo(dirPath);

    if (fileInfo.isDir())
    {
        QDir curDir(dirPath);
        QStringList entries = curDir.entryList();

        // we iterate thru directory entries
        for (QStringList::iterator i = entries.begin(); i != entries.end(); i++)
        {
            // and process only proper entries
            QString entry = *i;

            if (entry.compare(".") && entry.compare(".."))
            {
                // the full path is pushed back
                QString path = dirPath + "/" + entry;
                filePaths->push_back(path);

                // notify ui
                QString statusStr = "Building a list of items for upload (" + QVariant((int)filePaths->size()).toString() + ")...";
                progressMarquee(statusStr);

                QFileInfo entInfo(path);

                // on directories, we recurse
                if (entInfo.isDir())
                    makeBillOfMaterials(path, filePaths);
            }
        }
    }
}
