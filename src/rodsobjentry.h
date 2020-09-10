/**
 * @file rodsobjentry.h
 * @brief Definition of Kanki library class RodsObjEntry
 *
 * The Kanki class RodsObjEntry represents an iRODS object entry in iCAT.
 *
 * Copyright (C) 2016-2020 KTH Royal Institute of Technology. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * Copyright (C) 2014-2016 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

#ifndef RODSOBJENTRY_H
#define RODSOBJENTRY_H

// C++ standard library headers
#include <string>

// boost library headers
#include "boost/shared_ptr.hpp"

// iRODS client library headers
#include "rodsClient.h"
#include "rodsPath.h"

namespace Kanki {

class RodsObjEntry
{

public:

    // constructor requires all the object properties as arguments and instantiates it fully defined
    RodsObjEntry(const std::string &theObjName, const std::string &theCollPath,
		 const std::string &theCreateTime, const std::string &theModifyTime,
		 objType_t theObjType, int theReplNum, int theReplStatus, rodsLong_t theObjSize);

    // constructs a fully qualified path for the iRODS object
    std::string getObjectFullPath();

    // constructs the base path for the iRODS object (parent collection path)
    std::string getObjectBasePath();

    // constructs the object name for the iRODS object (collection or data obj name)
    std::string getObjectName();

    // static class constants for iRODS object types (collection and data object)
    static const char *DataObjType;
    static const char *CollObjType;

    // iRODS object entry properties
    std::string objName;                // object name
    std::string collPath;               // collection path
    std::string createTime;             // object creation time (UNIX)
    std::string modifyTime;             // object modify time (UNIX)
    std::string chkSum;                 // object checksum
    objType_t objType;                  // object type
    int replNum;                        // replica number
    int replStatus;                     // replication status
    rodsLong_t objSize;                 // object size
};

// define smart pointer type for object entry
using RodsObjEntryPtr = std::shared_ptr<RodsObjEntry>;

// define container type for object array
using RodsObjArray = std::vector<Kanki::RodsObjEntryPtr>;

} // namespace Kanki

#endif // RODSOBJENTRY_H
