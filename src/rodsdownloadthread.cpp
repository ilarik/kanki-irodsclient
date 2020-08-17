/**
 * @file rodsdownloadthread.h
 * @brief Implementation of class RodsDownloadThread
 *
 * The RodsDownloadThread class extends the Qt thread management class
 * QThread and implements a worker thread for a download (get) operation.
 *
 * Copyright (C) 2016-2020 KTH Royal Institute of Technology. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * Copyright (C) 2014-2016 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

// application class RodsDownloadThread header
#include "rodsdownloadthread.h"

RodsDownloadThread::RodsDownloadThread(Kanki::RodsSession *theSession, Kanki::RodsObjEntryPtr theObj,
                                       const std::string &theDestPath, bool verifyChecksum, bool allowOverwrite)
    : QThread(),
      session(theSession),
      objEntry(theObj),
      destPath(theDestPath),
      verify(verifyChecksum),
      overwrite(allowOverwrite)
{
}

void RodsDownloadThread::run()
{
    int status = 0;

    // instantiate a worker thread pool for this task
    irods::thread_pool thr_tank(Kanki::RodsSession::numThreads);

    // setup progress display for the task
    QString statusStr = "Initializing...";
    mainProgressMarquee(statusStr);

    // in the case of downloading a collection, do it recursively
    if (this->objEntry->objType == COLL_OBJ_T)
    {
        std::vector<Kanki::RodsObjEntryPtr> collObjs;
        std::string basePath = this->objEntry->collPath;

        // first item to the download object list
        collObjs.push_back(this->objEntry);

        if ((status = makeCollObjList(this->objEntry, &collObjs)) < 0)
        {
	    reportError("Download failed", this->objEntry->getObjectFullPath().c_str(), status);
	}
	
	else {
            // notify ui of progress bar state (object count)
            setupMainProgress(statusStr, 0, collObjs.size());

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
                    statusStr = "Downloading ";
                    statusStr += curObj->getObjectName().c_str();

                    // notify ui of current operation and progress
                    mainProgressUpdate(statusStr, i+1);

                    // get a connection and a thread to execute a lambda
		    irods::thread_pool::post(thr_tank, [=, &status] {
			    Kanki::RodsSession::connection_proxy conn = this->session->getConnection();
			    if (!conn)
			    	reportError("iRODS connection failure", curObj->getObjectFullPath().c_str(), 
					    SYS_SOCK_CONNECT_ERR);
			    if ((status = this->getObject(conn, curObj, dstPath, this->verify, this->overwrite)) < 0)
				reportError("iRODS data stream error", curObj->getObjectFullPath().c_str(), status);
			});
                }

                // for collection objects we create the corresponding directory
                else if (curObj->objType == COLL_OBJ_T)
                {
                    // get directory name for ui
                    std::string dirName = dstPath.substr(dstPath.find_last_of('/') + 1);

                    // notify ui
                    statusStr = "Creating directory ";
                    statusStr += dirName.c_str();
                    mainProgressUpdate(statusStr, i+1);

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
        setupMainProgress(statusStr, 1, 1);

	// acquire a free connection from the pool
	Kanki::RodsSession::connection_proxy conn = this->session->getConnection();
	
	// we check the connection
	if (!conn)
	    reportError("iRODS connection failure", this->objEntry->getObjectFullPath().c_str(),
			SYS_SOCK_CONNECT_ERR);

        // we try a rods get operation
        else if ((status = this->getObject(conn, objEntry, dstPath, this->verify, this->overwrite)) < 0)
            reportError("iRODS data stream error", this->objEntry->getObjectFullPath().c_str(), status);
    }

    // wait for all workers to finish
    thr_tank.join();
}

int RodsDownloadThread::makeCollObjList(Kanki::RodsObjEntryPtr obj, std::vector<Kanki::RodsObjEntryPtr> *objs)
{
    int status = 0;

    // we proceed only for collections
    if (obj->objType == COLL_OBJ_T)
    {
        std::vector<Kanki::RodsObjEntryPtr> curCollObjs;

        // try to read collection
        if ((status = this->session->readColl(obj->collPath, &curCollObjs)) < 0)
	{
	    return (status);
	}
        
	else {
            // iterate thru current collection
            for (std::vector<Kanki::RodsObjEntryPtr>::iterator i = curCollObjs.begin(); i != curCollObjs.end(); i++)
            {
                // add object to list
                Kanki::RodsObjEntryPtr curObj = *i;
                objs->push_back(curObj);

                // notify ui
                QString statusStr = "Building a list of objects (" + QVariant((int)objs->size()).toString() + ")...";
                mainProgressMarquee(statusStr);

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

int RodsDownloadThread::getObject(irods::connection_pool::connection_proxy &conn,
				  Kanki::RodsObjEntryPtr obj, std::string localPath,
				  bool verifyChecksum, bool allowOverwrite)
{
    int status = 0;

    // check if we're allowed to proceed
    if (boost::filesystem::exists(localPath) && !allowOverwrite)
        return (OVERWRITE_WITHOUT_FORCE_FLAG);

    // take up namespaces
    namespace io = irods::experimental::io;
    namespace fs = irods::experimental::filesystem;

    // bring in a transport and a stream
    io::client::default_transport xport(conn);
    io::idstream inStream(xport, obj->getObjectFullPath());

    if (!inStream)
	return (SYS_API_INPUT_ERR);

    std::ofstream outStream(localPath, std::ofstream::binary | 
			    std::ofstream::out);
    
    if (!outStream)
	return (FILE_OPEN_ERR);

    // update status display only on large enough objects
    // TODO: FIXME: __KANKI_BUFSIZE_INIT !
    if (obj->objSize > __KANKI_BUFSIZE_INIT)
        setupSubProgress(obj->getObjectFullPath().c_str(), "Downloading...", 0, 100);

    // transfer the object via the stream
    // TODO: do parellel transfer if requested
    status = transferFileStream(obj, inStream, outStream);
    
    // close the local stream and rods data stream
    outStream.close();
    inStream.close();

    // verify checksum if needed
    if (verifyChecksum)
    {
	std::vector<fs::checksum> chksums = fs::client::data_object_checksum(conn, obj->getObjectFullPath(), 
									     fs::replica_number::all);
	if (chksums.size())
	{
	    fs::checksum &first_chksum = chksums.front();
	    
	    subProgressMarquee(obj->getObjectFullPath().c_str(), "Verifying Checksum...");
	    status = verifyChksumLocFile((char*)localPath.c_str(), first_chksum.value.c_str(), NULL);
	}
    }

    subProgressFinalize(obj->getObjectFullPath().c_str());

    // TODO: FIXME?
    //inStream.getOprEnd();

    return (status);
 }

int RodsDownloadThread::transferFileStream(Kanki::RodsObjEntryPtr obj, std::istream &inStream, std::ofstream &outStream)
{
    int status = 0;
    long int lastRead = 0, totalRead = 0, readSize = __KANKI_BUFSIZE_MAX;

    std::thread *writer = nullptr;
    char *buffer = nullptr, *buffer2 = nullptr;

    if ((buffer = (char*)std::malloc(readSize)) == nullptr || 
	(buffer2 = (char*)std::malloc(readSize)) == nullptr)
    {
	return (SYS_MALLOC_ERR);
    }
    
    // take namespaces and types
    namespace chrono = std::chrono;
    using clock = chrono::high_resolution_clock;

    using millisec_type = chrono::milliseconds;
    using timepoint_type = clock::time_point;

    // we measure time from the get-go
    timepoint_type t0 = clock::now();

    while (inStream && outStream)
    {
	inStream.read(buffer, readSize);
	lastRead = inStream.gcount();

	// time diff after block transfer
        timepoint_type t1 = clock::now();
        millisec_type diff = chrono::duration_cast<millisec_type>(t1 - t0);

        totalRead += lastRead;

	// if we had a thread for writing, wait for it
        if (writer)
        {
            writer->join();

            delete (writer);
	    writer = nullptr;

            // check for write errors
            if (outStream.rdstate () & std::ofstream::badbit)
            {
		status = FILE_WRITE_ERR;
                reportError("Download failed", "Write error", status);

                break;
            }
        }

        // (re)new writer thread and push a lambda function into it
        writer = new std::thread([&outStream, buffer, &lastRead] { 
		outStream.write(buffer, lastRead);
	    }); 

        // swap buffer pointers for double buffering (XOR swap)
        buffer = (char*)((uintptr_t)buffer ^ (uintptr_t)buffer2);
        buffer2 = (char*)((uintptr_t)buffer ^ (uintptr_t)buffer2);
        buffer = (char*)((uintptr_t)buffer ^ (uintptr_t)buffer2);

        // compute and signal statistics to UI
        double speed = ((double)totalRead / 1048576) / ((double)diff.count() / 1000);
        double percentage = ceil(((double)totalRead / (double)obj->objSize) * 100);

        QString statusStr = "Transferring... " + QVariant((int)percentage).toString() + "%";
        statusStr += " at " + QString::number(speed, 'f', 2) + " MB/s";

        if (obj->objSize > __KANKI_BUFSIZE_INIT)
            subProgressUpdate(obj->getObjectFullPath().c_str(), statusStr, (int)percentage);
    }

    // free everything we have allocated
    if (writer)
    {
	writer->join();
	delete (writer);
    }

    std::free(buffer);
    std::free(buffer2);

    return (status);
}

int RodsDownloadThread::transferFileParallel(Kanki::RodsObjEntryPtr obj, std::istream &inStream, std::ofstream &outStream)
{
    // TODO: FIXME!
    return (SYS_NOT_SUPPORTED);
}
