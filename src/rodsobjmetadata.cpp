/**
 * @file rodsobjmetadata.cpp
 * @brief Implementation of Kanki library class RodsObjMetadata
 *
 * The Kanki class RodsObjMetadata implements a container for iRODS
 * object AVU metadata and interfaces for metadata updates.
 *
 * Copyright (C) 2016-2020 KTH Royal Institute of Technology. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * Copyright (C) 2014-2016 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

// Kanki library class RodsObjMetadata header
#include "rodsobjmetadata.h"

namespace Kanki {

const char *RodsObjMetadata::AddOperation = "add";
const char *RodsObjMetadata::ModOperation = "mod";
const char *RodsObjMetadata::RmOperation = "rm";

RodsObjMetadata::RodsObjMetadata(Kanki::RodsSession *theSession, RodsObjEntryPtr theObjEntry)
{
    this->session = theSession;
    this->objEntry = theObjEntry;
}

int RodsObjMetadata::refresh()
{
    int status = 0;
    std::string queryString;

    if (this->objEntry->objType == DATA_OBJ_T)
    	queryString = fmt::format("SELECT META_DATA_ATTR_NAME, "
				  "META_DATA_ATTR_VALUE, "
				  "META_DATA_ATTR_UNITS "
				  "WHERE DATA_NAME = '{}' AND COLL_NAME = '{}'",
				  this->objEntry->objName,
				  this->objEntry->collPath);
   
    else
	queryString = fmt::format("SELECT META_COLL_ATTR_NAME, "
				  "META_COLL_ATTR_VALUE, "
				  "META_COLL_ATTR_UNITS "
				  "WHERE COLL_NAME = '{}'",
				  this->objEntry->collPath);

    using qb = irods::experimental::query_builder;

    try {
        this->attrValues.clear();
        this->attrUnits.clear();

	using row_type = irods::query<rcComm_t>::value_type;

	for (const row_type &row : qb().build(*(this->session->commPtr()), queryString))
	    this->addToStore(row.at(0), row.at(1), row.at(2));
    } catch (const irods::exception &e) {
	status = e.code();
    } catch (const std::exception &e) {
	status = SYS_INTERNAL_ERR;
    }

    // the metadata refresh status is returned
    return (status);
}

void RodsObjMetadata::addToStore(std::string name, std::string value, std::string unit)
{
    this->addToKeyVals(&this->attrValues, name, value);
    this->addToKeyVals(&this->attrUnits, name, unit);
}

void RodsObjMetadata::addToKeyVals(RodsObjMetadata::KeyVals *keyValsPtr, std::string key, std::string value)
{
    // if there is no value vector for this key in the hastable
    if (keyValsPtr->find(key) == keyValsPtr->end())
    {
        // initialize hashtable entry with an empty vector object
        (*keyValsPtr)[key] = std::vector<std::string>();
    }

    // the value is pushed to the end of the value vector
    keyValsPtr->at(key).push_back(value);
}

void RodsObjMetadata::removeFromKeyVals(RodsObjMetadata::KeyVals *keyValsPtr, std::string key, std::string value)
{
    // try to find value vector
    if (keyValsPtr->find(key) != keyValsPtr->end())
    {
        // get reference to value store vector
        std::vector<std::string> &values = keyValsPtr->at(key);

        // iterate thru value store vector to find instance
        for (std::vector<std::string>::iterator i = values.begin(); i != values.end(); i++)
        {
            // if found, erase instance from the vector
            if (!value.compare(*i))
                values.erase(i);
        }
    }
}

const RodsObjMetadata::KeyVals& RodsObjMetadata::getValues() const
{
    // return a constant reference of the hashtable
    return (this->attrValues);
}

const RodsObjMetadata::KeyVals& RodsObjMetadata::getUnits() const
{
    // return a constant reference of the hashtable
    return (this->attrUnits);
}

int RodsObjMetadata::addAttribute(std::string attrName, std::string attrValue, std::string attrUnit)
{
    modAVUMetadataInp_t mod;
    std::string objPath;
    int status = 0;

    // get object full path
    objPath = this->objEntry->getObjectFullPath();

    // initialize iRODS API metadata modification struct
    mod.arg0 = (char*)RodsObjMetadata::AddOperation;
    mod.arg1 = (char*)(this->objEntry->objType == DATA_OBJ_T ? RodsObjEntry::DataObjType : RodsObjEntry::CollObjType);
    mod.arg2 = (char*)objPath.c_str();
    mod.arg3 = (char*)attrName.c_str();
    mod.arg4 = (char*)attrValue.c_str();
    mod.arg5 = (char*)attrUnit.c_str();
    mod.arg6 = (char*)"";
    mod.arg7 = (char*)"";
    mod.arg8 = (char*)"";
    mod.arg9 = (char*)"";

    // execute metadata add thru rods api
    if ((status = rcModAVUMetadata(this->session->commPtr(), &mod)) < 0)
    {
        // return error code to caller
        return (status);
    }

    // update internal structures
    else {
        this->addToStore(attrName, attrValue, attrUnit);
    }

    // otherwise successful
    return (status);
}

int RodsObjMetadata::modifyAttribute(std::string attrName, std::string valueStr, std::string newValueStr,
                                     std::string unitStr, std::string newUnitStr)
{
    modAVUMetadataInp_t mod;
    std::string objPath, valueArg, unitArg;
    int status = 0;

    // get object full path
    objPath = this->objEntry->getObjectFullPath();

    // construct value modify argument string
    valueArg = "v:" + newValueStr;

    // if the update is being modified as well
    if (unitStr.length() > 0)
    {
        unitArg = "u:" + newUnitStr;
    }

    // initialize iRODS API metadata modification struct
    mod.arg0 = (char*)RodsObjMetadata::ModOperation;
    mod.arg1 = (char*)(this->objEntry->objType == DATA_OBJ_T ? RodsObjEntry::DataObjType : RodsObjEntry::CollObjType);
    mod.arg2 = (char*)objPath.c_str();
    mod.arg3 = (char*)attrName.c_str();
    mod.arg4 = (char*)valueStr.c_str();

    // set arguments for unit modify if needed
    if (unitStr.length() > 0)
    {
        mod.arg5 = (char*)unitStr.c_str();
        mod.arg6 = (char*)valueArg.c_str();
        mod.arg7 = (char*)unitArg.c_str();
        mod.arg8 = (char*)"";
        mod.arg9 = (char*)"";
    }

    // otherwise just the value is modified
    else {
        mod.arg5 = (char*)valueArg.c_str();
        mod.arg6 = (char*)"";
        mod.arg7 = (char*)"";
        mod.arg8 = (char*)"";
        mod.arg9 = (char*)"";
    }

    // execute metadata update thru rods api
    if ((status = rcModAVUMetadata(this->session->commPtr(), &mod)) < 0)
    {
        // in the case of failure, return error code
        return (status);
    }

    // update internal data structures accordingly
    else {
        // take references to value and unit vectors
        std::vector<std::string> &values = this->attrValues[attrName];
        std::vector<std::string> &units = this->attrUnits[attrName];

        // find the correct AVU triplet from the vectors
        for (unsigned int i = 0; i < values.size() && i < units.size(); i++)
        {
            // if a match was found, value equals and if unit is defined it equals
            if (!values.at(i).compare(valueStr) && units.at(i).length() && !units.at(i).compare(unitStr))
            {
                // modify attribute
                values.at(i) = newValueStr;
                units.at(i) = newUnitStr;

                // and we are done
                return (status);
            }
        }

        // in the case we are corrupt, rectify situation
        values.push_back(newValueStr);
        units.push_back(newUnitStr);
    }

    // we were successful
    return (status);
}

int RodsObjMetadata::removeAttribute(std::string attrName, std::string valueStr, std::string unitStr)
{
    modAVUMetadataInp_t remove;
    std::string objPath;
    int status = 0;

    // get object path
    objPath = this->objEntry->getObjectFullPath();

    // initialize iRODS API metadata modification struct
    remove.arg0 = (char*)RodsObjMetadata::RmOperation;
    remove.arg1 = (char*)(this->objEntry->objType == DATA_OBJ_T ? RodsObjEntry::DataObjType : RodsObjEntry::CollObjType);
    remove.arg2 = (char*)objPath.c_str();
    remove.arg3 = (char*)attrName.c_str();
    remove.arg4 = (char*)valueStr.c_str();
    remove.arg5 = (char*)unitStr.c_str();
    remove.arg6 = (char*)"";
    remove.arg7 = (char*)"";
    remove.arg8 = (char*)"";
    remove.arg9 = (char*)"";

    // execute metadata removal thru rods api
    if ((status = rcModAVUMetadata(this->session->commPtr(), &remove)) < 0)
    {
        return (status);
    }

    // update internal data structures
    else {
        // take references to value and unit vectors
        std::vector<std::string> &values = this->attrValues[attrName];
        std::vector<std::string> &units = this->attrUnits[attrName];

        // find the correct AVU triplet from the vectors
        for (unsigned int i = 0; i < values.size() && i < units.size(); i++)
        {
            // if a match was found, value equals and if unit is defined it equals
            if (!values.at(i).compare(valueStr) && units.at(i).length() && !units.at(i).compare(unitStr))
            {
                // get iterators for entries to be erased and erase
                std::vector<std::string>::iterator value = values.begin() + i;
                std::vector<std::string>::iterator unit = units.begin() + i;

                values.erase(value);
                units.erase(unit);
            }
        }
    }

    return (status);
}

} // namespace Kanki
