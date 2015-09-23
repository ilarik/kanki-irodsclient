/**
 * @file rodsobjmetadata.h
 * @brief Definition of Kanki library class RodsObjMetadata
 *
 * The Kanki class RodsObjMetadata implements a container for iRODS
 * object AVU metadata and interfaces for metadata updates.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

#ifndef KANKI_RODSOBJMETADATA_H
#define KANKI_RODSOBJMETADATA_H

// C++ standard library headers
#include <iostream>
#include <string>
#include <vector>
#include <map>

// iRODS client library headers
#include "rodsClient.h"
#include "rodsPath.h"

// Kanki iRODS C++ class library headers
#include "rodsconnection.h"
#include "rodsobjentry.h"
#include "_rodsgenquery.h"

namespace Kanki {

class RodsObjMetadata
{

public:

    // Constructor requires pointers to a Kanki rods connection object and to
    // and Kanki rods object entry.
    RodsObjMetadata(RodsConnection *theConn, RodsObjEntryPtr theObjEntry);

    // Class local type for a Key Value struct with multiple values per key.
    typedef std::map< std::string, std::vector< std::string > > KeyVals;

    // Interface for refreshing the contents of the metadata object from the iRODS iCAT.
    int refresh();

    // Interface for adding a metadata attribute AVU triplet to the metadata. Updates iRODS
    // iCAT and and if successful, updates metadata object internal storage to reflect the addition.
    // Returns rods api status code.
    int addAttribute(std::string attrName, std::string attrValue, std::string attrUnit = std::string());

    // Interface for updating a metadata attribute AVU triplet in the rods object metadata. Updates
    // iRODS iCAT and if successful, updates metadata object internal storage accordingly.
    // Returns rods api status code.
    int modifyAttribute(std::string attrName, std::string valueStr, std::string newValueStr,
                        std::string unitStr = std::string(), std::string newUnitStr = std::string());

    // Interface for removing a metadata attribute AVU triplet from the rods object metadata. Updates
    // iRODS iCAT and if successful, updates metadata object internal storage accordingly.
    // Returns rods api status code.
    int removeAttribute(std::string attrName, std::string valueStr, std::string unitStr);

    // Interface to obtain a constant reference to rods object metadata storage value container. TODO: better interface.
    const RodsObjMetadata::KeyVals& getValues() const;

    // Interface to obtain a constant reference to rods object metadata storage unit container. TODO: better interface.
    const RodsObjMetadata::KeyVals& getUnits() const;

private:

    // adds to metadata object internal store
    void addToStore(std::string name, std::string value, std::string unit = std::string());

    // adds to keyvals container
    void addToKeyVals(RodsObjMetadata::KeyVals *keyValsPtr, std::string key, std::string value);

    // removes from keyvals container
    void removeFromKeyVals(RodsObjMetadata::KeyVals *keyValsPtr, std::string key, std::string value);

    // pointer to Kanki rods connection object
    Kanki::RodsConnection *conn;

    // pointer to Kanki rods object entry object
    Kanki::RodsObjEntryPtr objEntry;

    // internal storage containers for metadata attribute values and units
    RodsObjMetadata::KeyVals attrValues;
    RodsObjMetadata::KeyVals attrUnits;

    // static class constants for rods api metadata operation codes
    static const char *AddOperation;
    static const char *ModOperation;
    static const char *RmOperation;
};

} // namespace Kanki

#endif // KANKI_RODSOBJMETADATA_H
