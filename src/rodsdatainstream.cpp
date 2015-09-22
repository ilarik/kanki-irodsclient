/**
 * @file rodsdatainstream.cpp
 * @brief Implementation of Kanki library class RodsDataInStream
 *
 * The RodsDataInStream class in Kanki provides an interface to the
 * iRODS protocol streaming read operations.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

// Kanki library class RodsDataInStream header
#include "rodsdatainstream.h"

namespace Kanki {

RodsDataInStream::RodsDataInStream(RodsConnection *theConn, RodsObjEntryPtr theObjEntry)
    : RodsDataStream(theConn)
{
    this->entPtr = theObjEntry;
}

int RodsDataInStream::openDataObj()
{
    dataObjInp_t openParam;
    int openResult = 0;

    std::memset(&openParam, 0, sizeof (openParam));

    // we open an object at the path read only
    openParam.openFlags = O_RDONLY;
    rstrcpy(openParam.objPath, this->entPtr->getObjectFullPath().c_str(), MAX_NAME_LEN);

    // try to execute rods api call
    if ((openResult = rcDataObjOpen(this->connPtr->commPtr(), &openParam)) < 0)
        return (openResult);

    // on success we have a first class object index
    else
        this->rodsL1Inx = openResult;

    return (openResult);
}

int RodsDataInStream::initGetOpr()
{
    dataObjInp_t getParam;
    bytesBuf_t getBuffer;
    portalOprOut_t *portalOpr = NULL;
    int status = 0;

    // zero rods api params
    memset(&getParam, 0, sizeof (getParam));
    memset(&getBuffer, 0, sizeof (getBuffer));

    // set rods api params for a get api operation
    getParam.oprType = GET_OPR;
    getParam.numThreads = NO_THREADING;
    rstrcpy(getParam.objPath, this->entPtr->getObjectFullPath().c_str(), MAX_NAME_LEN);

    // we always ask for the checksum and deny para oprs
    addKeyVal(&getParam.condInput, VERIFY_CHKSUM_KW, "");

#ifndef PARA_OPR
    addKeyVal(&getParam.condInput, NO_PARA_OP_KW, "");
#endif

    // try to initiate a get api request
    if ((status = procApiRequest(this->connPtr->commPtr(), DATA_OBJ_GET_AN, &getParam,
                                 NULL, (void**)&portalOpr, &getBuffer)) < 0)
        return (status);

    // in case we got object data
    if (portalOpr)
    {
        this->rodsL1Inx = portalOpr->l1descInx;
        this->objChecksum = portalOpr->chksum;

        std::free(portalOpr);
    }

    // free possibly allocated buffer
    if (getBuffer.buf)
        std::free(getBuffer.buf);

    return (status);
}

int RodsDataInStream::read(void *bufPtr, size_t len)
{
    openedDataObjInp_t readParam;
    bytesBuf_t readBuf;
    int readResult = 0;

    // set read params, level 1 index and read length
    std::memset(&readParam, 0, sizeof (readParam));
    readParam.l1descInx = this->rodsL1Inx;
    readParam.len = len;

    // we use the provided buffer
    std::memset(&readBuf, 0, sizeof (readBuf));

    // try to read from the rods data object
    if ((readResult = rcDataObjRead(this->connPtr->commPtr(), &readParam, &readBuf)))
    {
        std::memcpy(bufPtr, readBuf.buf, len);
        std::free(readBuf.buf);

        this->lastOprSize = readResult;
    }

    return (readResult);
}

int RodsDataInStream::read(size_t len)
{
    // if our buffer is too small, try to grow it
    if (this->bufSize < len)
    {
        // abort if requested size too big
        if ((this->growBuffer(len)) < len)
            return (-1);
    }

    return (this->read(this->memBuffer, len));
}

void RodsDataInStream::getOprDone()
{
    // complete client rods api get request
    rcOprComplete(this->connPtr->commPtr(), this->rodsL1Inx);
}

const std::string& RodsDataInStream::checksumStr() const
{
    return (this->objChecksum);
}

const char* RodsDataInStream::checksum() const
{
    return (this->objChecksum.c_str());
}

} // namespace Kanki
