/**
 * @file rodsobjentry.cpp
 * @brief Implementation of Kanki library class RodsObjEntry
 *
 * The Kanki class RodsObjEntry represents an iRODS object entry in iCAT.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

// Kanki library class RodsObjEntry header
#include "rodsobjentry.h"

namespace Kanki {

const char *RodsObjEntry::DataObjType = "-d";
const char *RodsObjEntry::CollObjType = "-C";

RodsObjEntry::RodsObjEntry(const std::string &theObjName, const std::string &theCollPath, const std::string &theCreateTime,
                           const std::string &theModifyTime, objType_t theObjType, int theReplNum, int theReplStatus, rodsLong_t theObjSize)
    : objName(theObjName), collPath(theCollPath), createTime(theCreateTime), modifyTime(theModifyTime), objType(theObjType),
      replNum(theReplNum), replStatus(theReplStatus), objSize(theObjSize)
{

}

std::string RodsObjEntry::getObjectFullPath()
{
    std::string objPath;

    // object path for collections is the object name
    if (this->objType == COLL_OBJ_T)
        objPath = this->objName;

    // for data objects we need to add to the path
    else if (this->objType == DATA_OBJ_T)
        objPath = this->collPath + "/" + this->objName;

    return (objPath);
}

std::string RodsObjEntry::getObjectBasePath()
{
    std::string basePath;

    // for collections, we need to get the parent path
    if (this->objType == COLL_OBJ_T)
    {
        basePath = this->collPath.substr(0, this->collPath.find_last_of('/'));
    }

    // for data objects, the base path is in the collection path
    else if (this->objType == DATA_OBJ_T)
        basePath = this->collPath;

    return (basePath);
}

std::string RodsObjEntry::getObjectName()
{
    std::string objName;

    // for collections we need to get the last part of path
    if (this->objType == COLL_OBJ_T)
        objName = this->collPath.substr(this->collPath.find_last_of('/') + 1);

    // for data objects its trivial
    else if (this->objType == DATA_OBJ_T)
        objName = this->objName;

    return (objName);
}

} // namespace Kanki
