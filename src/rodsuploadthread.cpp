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

RodsUploadThread::RodsUploadThread(Kanki::RodsConnection *theConn, QStringList filePaths, std::string destColl)
    : QThread()
{
    this->conn = theConn;
    this->filePathList = filePaths;
    this->destCollPath = destColl;
}

void RodsUploadThread::run() Q_DECL_OVERRIDE
{
    int status = 0;
    int c = 0;

    // signal ui to setup progress display
    setupProgressDisplay("Starting Upload...", 0, this->filePathList.count());

    // iterate trough Qt string list containing the file names
    for (QStringList::const_iterator i = this->filePathList.begin(); i != this->filePathList.end(); i++)
    {
        // get next file path to upload
        QString fileName = *i;

        // check whether current path is a directory
        QFileInfo fileInfo(fileName);

        // the file is a directory, make rods collection
        if (fileInfo.isDir())
        {
            std::string path = fileName.toStdString();
            std::string dirName = (char*)(strrchr(path.c_str(), '/'));
            std::string collPath = this->destCollPath + dirName;

            progressUpdate("Creating Collection " + fileName, c);

            if ((status = this->conn->makeColl(collPath, false)) < 0)
            {
                reportError("iRODS make collection error", "Put failed", status);
            }
       }

        // if the file is a regular file with read permissions, upload
        if (fileInfo.isFile() && fileInfo.isReadable())
        {
            std::string path = fileName.toStdString();
            std::string name = (char*)(strrchr(path.c_str(), '/') + 1);
            std::string objPath = this->destCollPath + "/" + name;

            std::string statusStr = "Uploading file " + name;
            progressUpdate(statusStr.c_str(), c);

            // try to put file and report possible error to user
            if ((status = this->conn->putFile(fileName.toStdString(), objPath)) < 0)
                reportError("iRODS put file error", "Put failed", status);
        }

        c++;
    }

    done();
}
