/**
 * @file rodsconnection.h
 * @brief Definition of Kanki library class RodsConnection
 *
 * The RodsConnection class in Kanki provides an interface to the
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

#ifndef RODSCONNECTION_H
#define RODSCONNECTION_H

// C++ standard library headers
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <mutex>
#include <cstdint>

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
#include "checksum.hpp"

#include "irods_auth_object.hpp"
#include "irods_auth_factory.hpp"
#include "irods_auth_plugin.hpp"
#include "irods_auth_manager.hpp"
#include "irods_auth_constants.hpp"
#include "irods_native_auth_object.hpp"
#include "irods_pam_auth_object.hpp"
#include "authPluginRequest.h"
#include "irods_kvp_string_parser.hpp"

// new-age iRODS headers
#include "filesystem.hpp"

// Kanki iRODS C++ class library headers
#include "rodsobjentry.h"

namespace Kanki {

class RodsConnection
{

public:

    // Constructor for instantiating a new connection object, optionally identical with
    // respect to the parameters of the conn object pointed by argument connPtr.
    RodsConnection(RodsConnection *connPtr = NULL);

    // Destructor to disconnect and clean up.
    ~RodsConnection();

    // Establishes an iRODS protocol connection to an iRODS server. Also configures the
    // connection object for connection parameters from the iRODS user environment if
    // no connection parameters have been provided.
    int connect();

    // For an established connection, executes iRODS protocol user authentication procedure
    // according to provided credentials. By default uses iRODS user environment.
    int login(const std::string &authScheme = "", const std::string &userName = "", const std::string &password = "");

    // Disconnects from the iRODS server.
    int disconnect(bool force = false);

    // Interface for querying whether the connection object is fully ready to be used, i.e.
    // connection was successful and user authentication is complete.
    bool isReady() const;

    // Interface for querying whether the iRODS connection is an SSL secured connection.
    bool isSSL() const;

    // Interface for accessing the iRODS comm pointer, for bypassing the Kanki RodsConnection
    // object interfaces - necessary for now.
    rcComm_t* commPtr() const;

    // Interface for accessing the SSL cipher info structure, provides a const pointer.
    const SSL_CIPHER* cipherInfo() const;

    // Interface for querying the iRODS username for the connection, provides a C++ std string
    // copy of the rods api provided C string.
    std::string rodsUser() const;

    // Interface for querying the iRODS server hostname of the connection, provides a C++ std string
    // copy of the rods api provided C string.
    std::string rodsHost() const;

    // Interface for querying the iRODS home collection of the session, provides a C++ std string
    // copy of the rods api provided C string.
    std::string rodsHome() const;

    // Interface for querying the iRODS home zone of the iRODS server, provides a C++ std string
    // copy of the rods api provided C string.
    std::string rodsZone() const;

    // Interface for querying the iRODS default resource of the user, provides a C++ std string
    // copy of the rods api provided C string.
    std::string rodsDefResc() const;

    // Interface for querying the iRODS authentication scheme used, provides a C++ std string copy
    // of the rods api provided C string.
    std::string rodsAuthScheme() const;

    // Interface for querying the last rods api provided error message, provides a C++ std string
    // copy of the rods api provided C string.
    std::string lastErrorMsg() const;

    // Interface for querying the last rods api provided error code.
    int lastError() const;

    // Locks the connection specific mutex lock to prevent simultaneous use of the same iRODS
    // connection by two different threads.
    void mutexLock();

    // Frees the connection specific mutex lock to make the iRODS TCP connection data stream
    // available for other threads.
    void mutexUnlock();

    // Reads an iRODS collection from the iRODS server to memory using the locally cached version
    // of rods api read collection function, parses the collection into a vector of RodsObjEntry objs.
    int readColl(const std::string &collPath, std::vector<Kanki::RodsObjEntryPtr> *collObjs);

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

    // 
    static const uint32_t numThreads = 32;

    //
    static const uint64_t xferBlkSize = 16777216;
    
private:

    // Authenticates the user against the iRODS server in a new connection.
    int authenticate(const std::string &authScheme = "", const std::string &userName = "", const std::string &password = "");

    // we deny assignments and copying of the object
    RodsConnection(RodsConnection &);
    RodsConnection& operator=(RodsConnection &);

    // a mutex to provide locking for the TCP connection data stream
    std::mutex commMutex;

    // rods api communications pointer
    rcComm_t *rodsCommPtr;

    // rods api user environment
    rodsEnv rodsUserEnv;

    // rods api last error message
    rErrMsg_t lastErrMsg;
};

} // namespace Kanki

#endif // RODSCONNECTION_H
