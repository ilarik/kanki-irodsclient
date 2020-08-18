/**
 * @file rodssession.cpp
 * @brief Implementation of Kanki library class RodsSession
 *
 * The RodsSession class in Kanki provides an interface to the
 * to an iRODS protocol connection and iRODS protocol operations.
 *
 * Copyright (C) 2016 KTH Royal Institute of Technology. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * Copyright (C) 2014-2016 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

// Kanki library class RodsSession header
#include "rodssession.h"

namespace Kanki {

RodsSession::RodsSession(const RodsSession *sessPtr)
{
    // initially we have no comm ptr
    this->rodsCommPtr = nullptr;

    // if we had a session to replicate
    if (sessPtr)
	memcpy(&(this->rodsUserEnv), &(sessPtr->rodsUserEnv), sizeof(rodsUserEnv));

    // zero local rods api data structures
    memset(&this->rodsUserEnv, 0, sizeof (rodsEnv));
    memset(&this->lastErrMsg, 0, sizeof (rErrMsg_t));

    // initialize iRODS 4.2 API tables new-age style
    load_client_api_plugins();

    // set client signature oldskool
    mySetenvStr(SP_OPTION, Kanki::RodsSession::signatureStr);

    // acquire thread pool
    thread_pool = std::make_unique<irods::thread_pool>(Kanki::RodsSession::numThreads);
}

RodsSession::~RodsSession()
{
    // forcefully disconnect if we are connected to rods
    if (this->rodsCommPtr)
        this->disconnect(true);
}

int RodsSession::connect()
{
    int status = 0;

    // if we are already connected, nothing to be done
    if (this->rodsCommPtr)
        return (status);

    std::lock_guard mutexGuard(this->commMutex);

    // get user iRODS environment
    if ((status = getRodsEnv(&this->rodsUserEnv)) < 0)
        return (status);

    try {
	this->conn_pool = std::make_unique<irods::connection_pool>(Kanki::RodsSession::numThreads,
								   this->rodsUserEnv.rodsHost,
								   this->rodsUserEnv.rodsPort,
								   this->rodsUserEnv.rodsUserName,
								   this->rodsUserEnv.rodsZone,
								   Kanki::RodsSession::refreshTime);
    }
    catch (const std::runtime_error &e) 
    {
	// TODO: FIXME!
	return (SYS_INTERNAL_ERR);
    }

    // take out legacy "default" connection
    this->rodsCommPtr = this->conn_pool->get_connection().release();

    // workaround for refreshing the connection pool
    {
	std::vector<connection_proxy> temp;
	for (int i = 0; i < (int)Kanki::RodsSession::numThreads; i++)
	    temp.push_back(this->conn_pool->get_connection());
    }
    
    return (status);
}

int RodsSession::login(const std::string &authScheme, const std::string &userName, const std::string &password)
{
    int status = 0;

    // first of all, we must be connected
    if (!this->rodsCommPtr)
        return (status);

    // login only if not already logged in
    if (!this->isReady())
    {
      std::lock_guard commGuard(this->commMutex);
      
      // try to authenticate client to the iRODS server
      if ((status = this->authenticate(authScheme, userName, password)) != 0)
        {
	  rcDisconnect(this->rodsCommPtr);
	  this->rodsCommPtr = nullptr;
        }
    }

    return (status);
}

int RodsSession::authenticate(const std::string &authScheme, const std::string &userName, const std::string &password)
{
    irods::error status;

    std::string scheme;
    std::string context;

    if (this->commPtr() == nullptr)
        return (-1);

    if (this->commPtr()->loggedIn)
        return (0);

    if (authScheme.empty())
        scheme = this->rodsUserEnv.rodsAuthScheme;
    else
        scheme = authScheme;

    std::transform(scheme.begin(), scheme.end(), scheme.begin(), ::tolower);

    irods::auth_object_ptr authObj;
    status = irods::auth_factory(scheme, this->rodsCommPtr->rError, authObj);

    if (!status.ok())
        return (status.code());

    irods::plugin_ptr pluginPtr;
    status = authObj->resolve(irods::AUTH_INTERFACE, pluginPtr);

    if (!status.ok())
        return (status.code());

    irods::auth_ptr authPlugin = boost::dynamic_pointer_cast<irods::auth>(pluginPtr);

    // handle native auth special case (password scramble)
    if (scheme == "native" && !password.empty())
        obfSavePw(0, 0, 0, password.c_str());


    // handle PAM auth special case (inject password into plugin context)
    // else if (scheme == "pam")
    // {
    //     irods::kvp_map_t keyValues;

    //     keyValues[irods::AUTH_TTL_KEY] = 3600;
    //     keyValues[irods::AUTH_PASSWORD_KEY] = password;

    //     context = irods::escaped_kvp_string(keyValues);
    //    //irods::pam_auth_object_ptr pamPtr = boost::dynamic_pointer_cast<
    // }

    status = authPlugin->call<rcComm_t*, const char*>(nullptr, irods::AUTH_CLIENT_START, authObj, this->rodsCommPtr, context.c_str());

    if (!status.ok())
        return (status.code());

    status = authPlugin->call<rcComm_t*>(nullptr, irods::AUTH_CLIENT_AUTH_REQUEST, authObj, this->rodsCommPtr);

    if (!status.ok())
        return (status.code());

    status = authPlugin->call(nullptr, irods::AUTH_ESTABLISH_CONTEXT, authObj);

    if (!status.ok())
        return (status.code());

    status = authPlugin->call<rcComm_t*>(nullptr, irods::AUTH_CLIENT_AUTH_RESPONSE, authObj, this->rodsCommPtr);

    if (!status.ok())
        return (status.code());

    // if we got here, success
    this->rodsCommPtr->loggedIn = 1;
    return (0);
}

int RodsSession::disconnect(bool force)
{
    int status = 0;

    // only if there is a connection (conn pointer set)
    if (this->rodsCommPtr)
    {
	const auto _disconnect = [&status, this] { 
	    // do rods api disconnect and set comm ptr to null
	    status = rcDisconnect(this->rodsCommPtr);
	    this->rodsCommPtr = nullptr;
	};
	
        // wait mutex (if needed) and disconnect
        if (!force)
	{
	    std::lock_guard commGuard(this->commMutex); 
	    _disconnect();
	}
	
	else _disconnect();
    }

    // return rods api status
    return (status);
}

int RodsSession::refreshResourceTable()
{
    int status = 0;

    std::string queryStr = "SELECT RESC_ID, RESC_PARENT, RESC_NAME, RESC_LOC, RESC_COMMENT";
	
    try {
	using query_builder = irods::experimental::query_builder;
	using row_type = irods::query<rcComm_t>::value_type;
	
	for (const row_type &row : query_builder().build(*(this->rodsCommPtr), queryStr))
	{
	    unsigned long id = row.at(0).size() ? std::stoul(row.at(0)) : 0;
	    unsigned long parent_id = row.at(1).size() ? std::stoul(row.at(1)) : 0;
	    
	    rescTable[id] = Kanki::RodsSession::Resource{id,
							 parent_id,
							 row.at(2),
							 row.at(3),
							 row.at(4)};
	}
    }
    catch (const irods::exception &e) 
    {
	status = e.code();
    }
    catch (const std::exception &e)
    {
	status = SYS_INTERNAL_ERR;
    }

    return (status);
}

int RodsSession::makeColl(const std::string &collPath, bool makeRecursive)
{
    int status = 0;

    // sanity check, input argument string must be nonempty and begin with /
    if (collPath.empty() || collPath.find_first_of('/') != 0)
        return (-1);

    std::lock_guard mutexGuard(this->commMutex);

    namespace fs = irods::experimental::filesystem;

    try {
	fs::client::create_collection(*(this->commPtr()), collPath);
    }
    catch (fs::filesystem_error &e) {
	status = e.code().value();
    }

    // return status to caller
    return (status);
}

int RodsSession::readColl(const std::string &collPath, std::vector<RodsObjEntryPtr> *collObjs)
{
    int status = 0;

    // sanity checks for arguments
    if (!collObjs || collPath.empty())
        return (-1);

    std::lock_guard mutexGuard(this->commMutex);
    
    namespace fs = irods::experimental::filesystem;

    using ObjEntry = Kanki::RodsObjEntry;
    using ObjEntryPtr = Kanki::RodsObjEntryPtr;

    try 
    {
	for (fs::collection_entry const &entry : fs::client::collection_iterator(*(this->commPtr()), collPath))
	{
	    ObjEntryPtr newEntry(new ObjEntry(entry.is_data_object() ? entry.path().object_name().c_str() : entry.path().c_str(),
					      entry.is_data_object() ? entry.path().parent_path().c_str() : entry.path().c_str(),
					      std::to_string(entry.creation_time().time_since_epoch().count()),
					      std::to_string(entry.last_write_time().time_since_epoch().count()),
					      entry.is_data_object() ? DATA_OBJ_T : COLL_OBJ_T,
					      0,
					      1,
					      entry.is_data_object() ? const_cast<fs::collection_entry&>(entry).data_object_size() : 0));

	    // TODO: fix with emplace_back!
	    collObjs->push_back(newEntry);
	}
    }
    catch (fs::filesystem_error &e)
    {
	status = e.code().value();
    }

    return (status);
}

int RodsSession::removeColl(const std::string &collPath)
{
    int status = 0;

    // sanity check, input argument string must be nonempty and begin with /
    if (collPath.empty() || collPath.find_first_of('/') != 0)
        return (-1);

    std::lock_guard mutexGuard(this->commMutex);

    namespace fs = irods::experimental::filesystem;

    try {
	fs::client::remove_all(*(this->commPtr()), collPath, fs::remove_options::no_trash); 
    }
    catch (fs::filesystem_error &e)
    {
	status = e.code().value();
    }

    // return status to caller
    return (status);
}

int RodsSession::putFile(const std::string &localPath, const std::string &objPath, const std::string &rodsResc,
                            bool verifyChecksum, bool allowOverwrite, unsigned int numThreads)
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

    std::lock_guard mutexGuard(this->commMutex);

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

    if (verifyChecksum)
        addKeyVal(&putParam.condInput, VERIFY_CHKSUM_KW, "");

    if (allowOverwrite)
        addKeyVal(&putParam.condInput, FORCE_FLAG_KW, "");

    // target storage resource, if defined
    if (rodsResc.length())
        addKeyVal(&putParam.condInput, DEST_RESC_NAME_KW, rodsResc.c_str());

    // take copy of the local file path for the rods api
    strcpy(filePath, localPath.c_str());

    // call rods api
    status = rcDataObjPut(this->rodsCommPtr, &putParam, filePath);

    // return rods api status
    return (status);
}

int RodsSession::putFile(const std::string &localPath, const std::string &objPath, bool verifyChecksum,
                            bool allowOverwrite, unsigned int numThreads)
{
    // do put file to user default resource
    std::string defResc = this->rodsDefResc();
    return (this->putFile(localPath, objPath, defResc, verifyChecksum, allowOverwrite, numThreads));
}

int RodsSession::removeObj(const std::string &objPath)
{
    int status = 0;

    namespace fs = irods::experimental::filesystem;
    fs::path thePath(objPath);
    
    if (!thePath.is_absolute())
	return (USER_INPUT_PATH_ERR);

    std::lock_guard mutexGuard(this->commMutex);

    try {
	fs::client::remove(*(this->commPtr()), thePath, fs::remove_options::no_trash); 
    }
    catch (fs::filesystem_error &e)
    {
	status = e.code().value();
    }

    // return status to caller
    return (status);
}

int RodsSession::getFile(const std::string &localPath, const std::string &objPath, bool verifyChecksum,
                            bool allowOverwrite, unsigned int numThreads)
{
    int status = 0;
    dataObjInp_t getParam;

    // sanity check, input argument string must be nonempty and begin with /
    if (objPath.empty() || objPath.find_first_of('/') != 0)
        return (-1);

    std::lock_guard mutexGuard(this->commMutex);

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

    // return rods api status
    return (status);
}

int RodsSession::moveObjToColl(RodsObjEntryPtr objEntry, const std::string &collPath)
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

int RodsSession::moveObjToColl(const std::string &objPath, objType_t objType, const std::string &collPath)
{
    int status = 0;

    return (status);
}

int RodsSession::renameObj(RodsObjEntryPtr objEntry, const std::string &newName)
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

    // we need a lock here
    std::lock_guard mutexGuard(this->commMutex);

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

} // namespace Kanki
