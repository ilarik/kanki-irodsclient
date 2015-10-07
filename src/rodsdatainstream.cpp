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

    this->adaptiveSize = __KANKI_BUFSIZE_INIT;

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

int RodsDataInStream::getOprInit()
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
        std::memcpy(bufPtr, readBuf.buf, readResult);
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

int RodsDataInStream::readAdaptive(void *bufPtr, size_t maxLen)
{
    int readRequest = this->adaptiveSize, readResult = 0;

    // execute a timed read at current request size
    std::chrono::high_resolution_clock::time_point t0 = std::chrono::high_resolution_clock::now();
    readResult = this->read(bufPtr, readRequest);
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    // get time differential in milliseconds
    std::chrono::milliseconds dt = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);

    // compute read speed (troughput) and difference
    long int speed = floor((double)readResult / ((double)dt.count() / 1000));

    if (this->speed_.size())
    {
        long int diff = speed - this->speed_[this->speed_.size() - 1];
        this->diff_.push_back(diff);
    }

    // we store previous transaction data
    this->dt_.push_back(dt);
    this->bytes_.push_back(readResult);
    this->speed_.push_back(speed);

    // and keep only n previous data points
    if (this->dt_.size() > __KANKI_ADAPTIVE_INT && this->bytes_.size() > __KANKI_ADAPTIVE_INT)
    {
        this->dt_.erase(this->dt_.begin());
        this->bytes_.erase(this->bytes_.begin());
        this->speed_.erase(this->speed_.begin());
        this->diff_.erase(this->diff_.begin());
    }

    // compute moving average of diffs for the 'time window'
    double avg = 0;

    for (unsigned int i = 0; i < this->diff_.size(); i++)
        avg += this->diff_[i];

    avg /= this->diff_.size();

    // of our moving average is at all positive, we increase adaptive read size
    if ((avg > 0) && (std::abs(avg) > 1.0e3))
    {
        if (this->adaptiveSize + __KANKI_BUFSIZE_INCR < maxLen)
            this->adaptiveSize += __KANKI_BUFSIZE_INCR;
    }

    // on a very negative average, we decrease the transfer size parameter
    if ((avg < 0) && (std::abs(avg) > 1.0e6))
    {
        if (this->adaptiveSize > __KANKI_BUFSIZE_INCR)
            this->adaptiveSize -= __KANKI_BUFSIZE_INCR;
    }

    return (readResult);
}

void RodsDataInStream::getOprEnd()
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
