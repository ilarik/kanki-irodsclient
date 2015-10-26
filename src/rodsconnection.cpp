/**
 * @file rodsconnection.cpp
 * @brief Implementation of Kanki library class RodsConnection
 *
 * The RodsConnection class in Kanki provides an interface to the
 * to an iRODS protocol connection and iRODS protocol operations.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

// Kanki library class RodsConnection header
#include "rodsconnection.h"

namespace Kanki {

RodsConnection::RodsConnection(RodsConnection *connPtr)
{
    // initially we have no comm ptr
    this->rodsCommPtr = NULL;

    // zero local rods api data structures
    memset(&this->rodsUserEnv, 0, sizeof (rodsEnv));
    memset(&this->lastErrMsg, 0, sizeof (rErrMsg_t));

    // initialize iRODS 4 API tables
    irods::api_entry_table &api_table = irods::get_client_api_table();
    irods::pack_entry_table &pack_table = irods::get_pack_table();

    init_api_table(api_table, pack_table);

    // if we have a 'parent' connection pointer
    if (connPtr)
    {
        // TODO: copy connection parameters
    }
}

int RodsConnection::connect()
{
    int status = 0;

    // if we are already connected, nothing to be done
    if (this->rodsCommPtr)
        return (status);

    this->mutexLock();

    // get user iRODS environment
    if ((status = getRodsEnv(&this->rodsUserEnv)) < 0)
        return (status);

    // rods api connect
    if ((rodsCommPtr = rcConnect(this->rodsUserEnv.rodsHost, this->rodsUserEnv.rodsPort,
                                this->rodsUserEnv.rodsUserName, this->rodsUserEnv.rodsZone,
                                 0, &this->lastErrMsg)) == NULL)
    {
        this->mutexUnlock();
        return(-1);
    }

    this->mutexUnlock();
    return (status);
}

int RodsConnection::login()
{
    int status = 0;

    // first of all, we must be connected
    if (!this->rodsCommPtr)
        return (status);

    // login only if not already logged in
    if (!this->isReady())
    {
        this->mutexLock();

        // try to authenticate client to the iRODS server
        if ((status = clientLogin (this->rodsCommPtr)) != 0)
        {
            rcDisconnect(this->rodsCommPtr);
            this->rodsCommPtr = NULL;
        }

        this->mutexUnlock();
    }

    return (status);
}

int RodsConnection::disconnect()
{
    int status = 0;

    // only if there is a connection (conn pointer set)
    if (this->rodsCommPtr)
    {
        this->mutexLock();

        // do rods api disconnect and set comm ptr to null
        status = rcDisconnect(this->rodsCommPtr);
        this->rodsCommPtr = NULL;

        this->mutexUnlock();
    }

    // return rods api status
    return (status);
}

bool RodsConnection::isReady() const
{
    // connection is ready if there is a connection pointer and login was successful
    if (this->rodsCommPtr)
        if (this->rodsCommPtr->loggedIn)
            return (true);

    // otherwise connection is not ready
    return (false);
}

bool RodsConnection::isSSL() const
{
    // if we have a ready connection, get SSL status
    if (this->isReady())
    {
        // if an OpenSSL handle exists
        if (this->rodsCommPtr->ssl)
            return (true);
    }

    // if OpenSSL handle is not available, return false
    return (false);
}

const SSL_CIPHER *RodsConnection::cipherInfo() const
{
    // if we have a valid OpenSSL handle
    if (this->isSSL())
    {
        // return pointer to OpenSSL cipher in use
        return (SSL_get_current_cipher(this->rodsCommPtr->ssl));
    }

    // if no SSL, just return NULL
    return (NULL);
}

rcComm_t* RodsConnection::commPtr() const
{
    // return rods api connection pointer
    return (this->rodsCommPtr);
}

std::string RodsConnection::rodsUser() const
{
    return std::string(this->rodsUserEnv.rodsUserName);
}

std::string RodsConnection::rodsHost() const
{
    return std::string(this->rodsUserEnv.rodsHost);
}

std::string RodsConnection::rodsHome() const
{
    return std::string(this->rodsUserEnv.rodsHome);
}

std::string RodsConnection::rodsZone() const
{
    return std::string(this->rodsUserEnv.rodsZone);
}

std::string RodsConnection::rodsDefResc() const
{
    return std::string(this->rodsUserEnv.rodsDefResource);
}

std::string RodsConnection::lastErrorMsg() const
{
    return std::string(this->lastErrMsg.msg);
}

int RodsConnection::lastError() const
{
    return (this->lastErrMsg.status);
}

int RodsConnection::makeColl(const std::string &collPath, bool makeRecursive)
{
    collInp_t theColl;
    int status = 0;

    this->mutexLock();

    // initialize rods api coll struct
    memset(&theColl, 0, sizeof (collInp_t));
    rstrcpy(theColl.collName, collPath.c_str(), MAX_NAME_LEN);

    // if a recursive operation is asked for, specify it
    if (makeRecursive)
        addKeyVal(&theColl.condInput, RECURSIVE_OPR__KW, "");

    // call rods api to make collection
    status = rcCollCreate(this->rodsCommPtr, &theColl);

    this->mutexUnlock();

    // return status to caller
    return (status);
}

int RodsConnection::readColl(const std::string &collPath, std::vector<RodsObjEntryPtr> *collObjs)
{
    collHandle_t rodsColl;
    collEnt_t rodsCollEntry;
    char collPathIn[MAX_NAME_LEN], collPathOut[MAX_NAME_LEN];
    int status = 0;

    // sanity checks for arguments
    if (!collObjs || collPath.empty())
        return (-1);

    // take a temp copy of the coll path string
    strcpy(collPathIn, collPath.c_str());

    this->mutexLock();

    // first the path string must be parsed by iRODS
    if ((status = parseRodsPathStr(collPathIn, &this->rodsUserEnv, collPathOut) < 0))
        return (status);

    // try to open collection from iRODS
    if ((status = rclOpenCollection(this->rodsCommPtr, collPathOut, 0, &rodsColl)) < 0)
        return (status);

    // read collection while there are objects to loop over
    do {
        status = rclReadCollection(this->rodsCommPtr, &rodsColl, &rodsCollEntry);

        if (status >= 0)
        {
            Kanki::RodsObjEntryPtr newEntry(new Kanki::RodsObjEntry(rodsCollEntry.objType == DATA_OBJ_T ? rodsCollEntry.dataName : rodsCollEntry.collName,
            rodsCollEntry.collName, rodsCollEntry.createTime, rodsCollEntry.modifyTime, rodsCollEntry.objType, rodsCollEntry.replNum,
            rodsCollEntry.replStatus, rodsCollEntry.dataSize));

            collObjs->push_back(newEntry);
        }

    } while (status >= 0);

    // close the collection handle
    status = rclCloseCollection(&rodsColl);

    this->mutexUnlock();

    return (status);
}

int RodsConnection::removeColl(const std::string &collPath)
{
    collInp_t theColl;
    int status = 0;

    // sanity check, input argument string must be nonempty and begin with /
    if (collPath.empty() || collPath.find_first_of('/') != 0)
        return (-1);

    this->mutexLock();

    // initialize rods api struct
    memset(&theColl, 0, sizeof (collInp_t));
    rstrcpy(theColl.collName, collPath.c_str(), MAX_NAME_LEN);

    // initialize parameters for remove operation
    addKeyVal(&theColl.condInput, RECURSIVE_OPR__KW, "");
    addKeyVal(&theColl.condInput, FORCE_FLAG_KW, "");

    // call for rods api to remove collection
    status = rcRmColl(this->rodsCommPtr, &theColl, false);

    this->mutexUnlock();

    // return status to caller
    return (status);
}

int RodsConnection::putFile(const std::string &localPath, const std::string &objPath, const std::string &rodsResc,
                            unsigned int numThreads)
{
    dataObjInp_t putParam;
    char filePath[MAX_NAME_LEN];
    rodsLong_t size = 0;
    int status = 0;

    // get file size
    std::ifstream fileStream(localPath.c_str(), std::ifstream::binary);
    fileStream.seekg(0, fileStream.end);
    size = fileStream.tellg();
    fileStream.close();

    this->mutexLock();

    // zero rods api input params structure
    memset(&putParam, 0, sizeof(dataObjInp_t));

    // initialize rods api input params struct
    putParam.dataSize = size;
    putParam.oprType = PUT_OPR;
    putParam.numThreads = numThreads;

    // set remote object path
    rstrcpy(putParam.objPath, objPath.c_str(), MAX_NAME_LEN);

    // for now, we use the generic data type
    addKeyVal(&putParam.condInput, DATA_TYPE_KW, "generic");

    // target storage resource, if defined
    if (rodsResc.length())
        addKeyVal(&putParam.condInput, DEST_RESC_NAME_KW, rodsResc.c_str());

    // take copy of the local file path for the rods api
    strcpy(filePath, localPath.c_str());

    // call rods api
    status = rcDataObjPut(this->rodsCommPtr, &putParam, filePath);

    this->mutexUnlock();

    // return rods api status
    return (status);
}

int RodsConnection::putFile(const std::string &localPath, const std::string &objPath, unsigned int numThreads)
{
    // do put file to user default resource
    std::string defResc = this->rodsDefResc();
    return (this->putFile(localPath, objPath, defResc, numThreads));
}

int RodsConnection::removeObj(const std::string &objPath)
{
    dataObjInp_t theObj;
    int status = 0;

    // sanity check, input argument string must be nonempty and begin with /
    if (objPath.empty() || objPath.find_first_of('/') != 0)
        return (-1);

    this->mutexLock();

    // initialize rods api struct
    memset(&theObj, 0, sizeof (dataObjInp_t));
    rstrcpy(theObj.objPath, objPath.c_str(), MAX_NAME_LEN);

    // initialize remove params
    addKeyVal(&theObj.condInput, FORCE_FLAG_KW, "");

    // call for rods api to remove data object
    status = rcDataObjUnlink(this->rodsCommPtr, &theObj);

    this->mutexUnlock();

    // return status to caller
    return (status);
}

int RodsConnection::getFile(const std::string &localPath, const std::string &objPath, bool verifyChecksum,
                            bool allowOverwrite, unsigned int numThreads)
{
    int status = 0;
    dataObjInp_t getParam;

    // sanity check, input argument string must be nonempty and begin with /
    if (objPath.empty() || objPath.find_first_of('/') != 0)
        return (-1);

    this->mutexLock();

    // zero rods api param struct
    memset(&getParam, 0, sizeof (dataObjInp_t));

    // set parameters for get operation
    getParam.oprType = GET_OPR;
    getParam.numThreads = numThreads;

    if (verifyChecksum)
        addKeyVal(&getParam.condInput, VERIFY_CHKSUM_KW, "");

    if (allowOverwrite)
        addKeyVal(&getParam.condInput, FORCE_FLAG_KW, "");

    // copy obj path string
    rstrcpy(getParam.objPath, objPath.c_str(), MAX_NAME_LEN);

    // execute data object get
    status = rcDataObjGet(this->rodsCommPtr, &getParam, (char*)localPath.c_str());

    this->mutexUnlock();

    // return rods api status
    return (status);
}

int RodsConnection::moveObjToColl(RodsObjEntryPtr objEntry, const std::string &collPath)
{
    int status = 0;
    dataObjCopyInp_t objMoveInp;
    std::string newObjPath;

    // construct dest object path starting from new coll path
    newObjPath = collPath;

    // check for trailing slash
    if (collPath.find_last_of('/') != collPath.length())
        newObjPath += "/";

    newObjPath += objEntry->getObjectName();

    // zero rods api param struct
    memset(&objMoveInp, 0, sizeof (dataObjCopyInp_t));

    // set operation type
    if (objEntry->objType == DATA_OBJ_T)
        objMoveInp.srcDataObjInp.oprType = objMoveInp.destDataObjInp.oprType = RENAME_DATA_OBJ;

    else if (objEntry->objType == COLL_OBJ_T)
        objMoveInp.srcDataObjInp.oprType = objMoveInp.destDataObjInp.oprType = RENAME_COLL;

    // set source and target object paths
    rstrcpy(objMoveInp.srcDataObjInp.objPath, objEntry->getObjectFullPath().c_str(), MAX_NAME_LEN);
    rstrcpy(objMoveInp.destDataObjInp.objPath, newObjPath.c_str(), MAX_NAME_LEN);

    // try to execute rods api call (which is named somewhat incorrectly...)
    if ((status = rcDataObjRename(this->rodsCommPtr, &objMoveInp)) >= 0)
    {
        // on success we update th object entry
        objEntry->collPath = collPath;

        if (objEntry->objType == COLL_OBJ_T)
            objEntry->objName = collPath;
    }

    // return rods api status to caller
    return (status);
}

int RodsConnection::renameObj(RodsObjEntryPtr objEntry, const std::string &newName)
{
    int status = 0;
    dataObjCopyInp_t objRenameInp;
    std::string newObjPath;

    // sanity check, if name doesn't change nothing to do
    if (!objEntry->getObjectName().compare(newName))
        return (status);

    // construct new dest object path
    newObjPath = objEntry->getObjectBasePath() + "/" + newName;

    // zero rods api param struct
    memset(&objRenameInp, 0, sizeof (dataObjCopyInp_t));

    // set operation type
    if (objEntry->objType == DATA_OBJ_T)
        objRenameInp.srcDataObjInp.oprType = objRenameInp.destDataObjInp.oprType = RENAME_DATA_OBJ;

    else if (objEntry->objType == COLL_OBJ_T)
        objRenameInp.srcDataObjInp.oprType = objRenameInp.destDataObjInp.oprType = RENAME_COLL;

    // set source and target object paths
    rstrcpy(objRenameInp.srcDataObjInp.objPath, objEntry->getObjectFullPath().c_str(), MAX_NAME_LEN);
    rstrcpy(objRenameInp.destDataObjInp.objPath, newObjPath.c_str(), MAX_NAME_LEN);

    // try to execute rods api call
    if ((status = rcDataObjRename(this->rodsCommPtr, &objRenameInp)) >= 0)
    {
        // on success we update object name
        if (objEntry->objType == DATA_OBJ_T)
            objEntry->objName = newName;

        // for collection objects update botn coll path and object name attribute
        else if (objEntry->objType == COLL_OBJ_T)
        {
            objEntry->collPath = objEntry->objName = newObjPath;
        }
    }

    return (status);
}

void RodsConnection::mutexLock()
{
    this->commMutex.lock();
}

void RodsConnection::mutexUnlock()
{
    this->commMutex.unlock();
}

} // namespace Kanki
