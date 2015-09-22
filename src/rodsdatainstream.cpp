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

int RodsDataInStream::read(void *extBuf, size_t len)
{
    openedDataObjInp_t readParam;
    bytesBuf_t readBuf;
    int readResult = 0;

    // if our buffer is too small, try to grow it
    if (this->bufSize < len)
    {
        // abort if requested size too big
        if ((this->growBuffer(len)) < len)
            return (-1);
    }

    // set read params, level 1 index and read length
    std::memset(&readParam, 0, sizeof (readParam));
    readParam.l1descInx = this->rodsL1Inx;
    readParam.len = len;

    // we use the stream buffer
    readBuf.len = this->bufSize;
    readBuf.buf = this->memBuffer;

    // try read, on success we copy to external buffer
    if ((readResult = rcDataObjRead(this->connPtr->commPtr(), &readParam, &readBuf)))
    {
        this->lastOprSize = len;
        std::memcpy(extBuf, this->memBuffer, len);
    }

    return (readResult);
}

} // namespace Kanki
