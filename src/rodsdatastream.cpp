/**
 * @file rodsdatastream.cpp
 * @brief Implementation of Kanki library class RodsDataStream
 *
 * The RodsDataStream class in Kanki provides an abstract interface
 * to the iRODS protocol streaming I/O operations.
 *
 * Copyright (C) 2016 KTH Royal Institute of Technology. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

// Kanki library class RodsDataStream header
#include "rodsdatastream.h"

namespace Kanki {

RodsDataStream::RodsDataStream(Kanki::RodsConnection *theConn)
{
    this->connPtr = theConn;

    this->memBuffer = std::malloc(__KANKI_BUFSIZE_INIT);
    this->bufSize = __KANKI_BUFSIZE_INIT;
    memset(this->memBuffer, 0, this->bufSize);
}

RodsDataStream::~RodsDataStream()
{
    std::free(this->memBuffer);
}

int RodsDataStream::seek(size_t offset, int whence)
{
    openedDataObjInp_t seekParam;
    fileLseekOut_t *seekResult = NULL;

    // zero param and results structs
    memset(&seekParam, 0, sizeof (seekParam));
    memset(&seekResult, 0, sizeof (seekResult));

    // set rods api seek parameters
    seekParam.offset = offset;
    seekParam.whence = whence;

    // try to seek and return status
    return (rcDataObjLseek(this->connPtr->commPtr(), &seekParam, &seekResult));
}

int RodsDataStream::closeDataObj()
{
    openedDataObjInp_t closeParam;
    int status = 0;

    // set first class object index to be closed
    std::memset(&closeParam, 0, sizeof (closeParam));
    closeParam.l1descInx = this->rodsL1Inx;

    // try to close, on success reset index
    if ((status = rcDataObjClose(this->connPtr->commPtr(), &closeParam)) >= 0)
        this->rodsL1Inx = 0;

    return (status);
}

size_t RodsDataStream::growBuffer(size_t newSize)
{
    if (newSize > __KANKI_BUFSIZE_MAX)
        newSize = __KANKI_BUFSIZE_MAX;

    if (!(this->memBuffer = std::realloc(this->memBuffer, newSize)))
        newSize = 0;

    this->bufSize = newSize;

    return (newSize);
}

} // namespace Kanki
