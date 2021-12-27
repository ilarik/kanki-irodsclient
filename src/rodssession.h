/**
 * @file rodssession.h
 * @brief Definition of Kanki library class RodsSession
 *
 * The RodsSession class in Kanki provides an interface to the
 * to an iRODS protocol session and iRODS protocol operations.
 *
 * Copyright (C) 2016-2020 KTH Royal Institute of Technology. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * Copyright (C) 2014-2016 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

#ifndef RODSSESSION_H
#define RODSSESSION_H

// C++ standard library headers
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <mutex>
#include <cstdint>
#include <memory>

// OpenSSL library headers
#include <openssl/ssl.h>

// boost library headers
#include <boost/thread/thread.hpp>
#include <boost/system/error_code.hpp>

// iRODS client library headers
#include "rodsClient.h"
#include "rodsPath.h"
#include "irods_client_api_table.hpp"
#include "irods_pack_table.hpp"
#include "checksum.h"

#include "irods_auth_object.hpp"
#include "irods_auth_factory.hpp"
#include "irods_auth_plugin.hpp"
#include "irods_auth_manager.hpp"
#include "irods_auth_constants.hpp"
#include "irods_native_auth_object.hpp"
#include "irods_pam_auth_object.hpp"
#include "authPluginRequest.h"
#include "irods_kvp_string_parser.hpp"

// fmt library
#include "fmt/format.h"

// new-age iRODS headers
#include "filesystem.hpp"
#include "transport/default_transport.hpp"
#include "thread_pool.hpp"
#include "connection_pool.hpp"
#include "query_builder.hpp"

// Kanki iRODS C++ class library headers
#include "rodsobjentry.h"

namespace Kanki {

class RodsSession
{

public:
    
    // === CONSTUCTORS AND DECONSTRUCTORS ===
    
    // constructor initializes a new session object, optionally cloning another one
    explicit RodsSession(const RodsSession *sessPtr = nullptr);
    explicit RodsSession(RodsSession &) = delete;    
    
    // we deny assignments and  moving of the object
    RodsSession& operator=(RodsSession &) = delete;
    
    // destructor cleans up and disconnects
    ~RodsSession();

    
    // === CLASS-LOCAL TYPE IMPORTS AND DEFINITIONS ===
    
    // import types from irods for connection pool
    using connection_pool = irods::connection_pool;
    using connection_pool_ptr = std::shared_ptr<connection_pool>;

    // import type for connection proxy
    using connection_proxy = connection_pool::connection_proxy;

    // import types for thread pool
    using thread_pool = irods::thread_pool;
    using thread_pool_ptr = std::unique_ptr<thread_pool>;
    
    // define local type for a storage resource
    struct Resource
    {
	unsigned long id;
	unsigned long parent_id;
	std::string name;
	std::string host;
	std::string comment;
    };
    
    // define local type for a resource table
    using ResourceTable = std::map<unsigned long, Kanki::RodsSession::Resource>;
    

    // === CLASS-LOCAL STATIC CONSTANTS ===

    // default values for some tunables
    static constexpr unsigned numThreads = 16;
    static constexpr unsigned refreshTime = 600;
    static constexpr unsigned minBlkSize = 262144;
    static constexpr unsigned long xferBlkSize = 16777216;
    
    // client signature string passed via the iRODS RPC API
    static constexpr char *signatureStr = "kanki";


    // === PUBLIC MEMBER FUNCTIONS ===
 
    // interface for accessing the iRODS comm pointer - necessary for now
    rcComm_t* commPtr() const { return (this->rodsCommPtr); }

    // locks the default-connection-specific mutex lock to prevent simultaneous use
    void mutexLock() { this->commMutex.lock(); }

    // frees the default-connection-specific mutex lock
    void mutexUnlock() { this->commMutex.unlock(); }

    // interface for querying the last rods api provided error code
    int lastError() const { return (this->lastErrMsg.status); }

    // interface for querying the last rods api provided error message
    std::string lastErrorMsg() const { return std::string(this->lastErrMsg.msg); }

    // returns a proxy object to an available iRODS connection from the pool
    connection_proxy getConnection() { return (this->connPool->get_connection()); }

    // schedules a task to the session-wide thread pool
    void scheduleTask(std::function<void()> callback) { thread_pool::post(*(this->tank), callback); }
    
    // gets a const reference to the resource table
    const ResourceTable& resourceTable() const { return (this->rescTable); }

    // interface for querying whether the session readiness
    bool isReady() const
    {
	// connection is ready if there is a connection pointer and login was successful
	if (this->rodsCommPtr && this->rodsCommPtr->loggedIn)
	    return (true);
	
	// otherwise connection is not ready
	return (false);
    }

    // interface for querying whether the iRODS session is using an SSL/TLS connections
    bool isSSL() const
    {
	// if we have a ready connection
	if (this->isReady() && this->rodsCommPtr->ssl)
	    return (true);
	
	// if OpenSSL handle is not available, return false
	return (false);
    }

    // interface for accessing the SSL cipher info structure, provides a const pointer
    const SSL_CIPHER* cipherInfo() const
    {
	// if we have a valid OpenSSL handle
	if (this->isReady() && this->isSSL())
	    return (SSL_get_current_cipher(this->rodsCommPtr->ssl));
	
	// if no SSL, return null pointer
	return (nullptr);
    }

    // getter for the iRODS username
    std::string rodsUser() const { return std::string(this->rodsUserEnv.rodsUserName); }
    
    // getter for iRODS endpoint hostname
    std::string rodsHost() const { return std::string(this->rodsUserEnv.rodsHost); }

    // getter for the iRODS home collection
    std::string rodsHome() const { return std::string(this->rodsUserEnv.rodsHome); }
    
    // getter for the iRODS home zone
    std::string rodsZone() const { return std::string(this->rodsUserEnv.rodsZone); }

    // getter for the iRODS default resource
    std::string rodsDefResc() const { return std::string(this->rodsUserEnv.rodsDefResource); }
    
    // getter for the iRODS authentication scheme
    std::string rodsAuthScheme() const { return std::string(this->rodsUserEnv.rodsAuthScheme); }

    // establishes iRODS protocol connections to an iRODS grid and configures session
    int connect();
    
    // executes iRODS protocol user authentication procedure depending on auth scheme
    int login(const std::string &authScheme = "",
	      const std::string &userName = "",
	      const std::string &password = "");
    
    // disconnects from the iRODS grid
    int disconnect(bool force = false);
  
    // refreshes the resource table from the catalog
    int refreshResourceTable();
    
    // reads an iRODS collection from the iRODS catalog into memory 
    int readColl(const std::string &collPath, RodsObjArray *collObjs,
		 rcComm_t *comm = nullptr);

    // Makes an iRODS collection at specified path, optionally recursively.
    int makeColl(const std::string &collPath, bool makeRecursive);

    // Removes an iRODS collection at specified path recursively.
    int removeColl(const std::string &collPath);

    // Puts a local file at localPath into iRODS as a data object at objPath to resource rodsResc
    // optionally using multithreaded iRODS transfer mode, defaults to one transfer thread.
    int putFile(const std::string &localPath, const std::string &objPath, const std::string &rodsResc,
                bool verifyChecksum, bool allowOverwrite, unsigned int numThreads = 1);

    // Puts a local file at localPAth into iRODS at objPath to the default resc
    int putFile(const std::string &localPath, const std::string &objPath, bool verifyChecksum,
                bool allowOverwrite, unsigned int numThreads = 1);

    // Gets an iRODS data object at objPath into a file located at localPath, optionally verifies
    // checksum and overwrites an existing file, defaults to one transfer thread.
    int getFile(const std::string &localPath, const std::string &objPath, bool verifyChecksum,
                bool allowOverwrite, unsigned int numThreads = 1);

    // Removes an iRODS data object at objPath.
    int removeObj(const std::string &objPath);

    // Moves an iRODS object represented by objEntry to collPath.
    int moveObjToColl(Kanki::RodsObjEntryPtr objEntry, const std::string &collPath);

    // Moves an iRODS object at objPath to collection at collPath.
    int moveObjToColl(const std::string &objPath, objType_t objType, const std::string &collPath);

    // Renames an iRODS object to newName.
    int renameObj(Kanki::RodsObjEntryPtr objEntry, const std::string &newName);
    

private:

    // === PRIVATE MEMBER FUNCTIONS === 

    // authenticates the user against the iRODS server in a new session
    int authenticate(const std::string &authScheme = "",
		     const std::string &userName = "", 
		     const std::string &password = "");


    // === PRIVATE CLASS MEMBERS ===

    // a mutex to provide locking for the TCP connection data stream
    std::mutex commMutex;

    // a thread pool for worker threads
    thread_pool_ptr tank;

    // a connection pool for iRODS connections
    connection_pool_ptr connPool;

    // hash table for storage resources
    ResourceTable rescTable;

    // rods api communications pointer
    rcComm_t *rodsCommPtr;

    // rods api user environment
    rodsEnv rodsUserEnv;

    // rods api last error message
    rErrMsg_t lastErrMsg;
};

} // namespace Kanki

#endif // RODSSESSION_H
