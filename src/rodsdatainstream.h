/**
 * @file rodsdatainstream.h
 * @brief Definition of Kanki library class RodsDataInStream
 *
 * The RodsDataInStream class in Kanki provides an interface to the
 * iRODS protocol streaming read operations.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

#ifndef RODSDATAINSTREAM_H
#define RODSDATAINSTREAM_H

#include <chrono>

#include "rodsconnection.h"
#include "rodsobjentry.h"
#include "rodsdatastream.h"

#define __KANKI_ADAPTIVE_INT    10

namespace Kanki {

class RodsDataInStream : public RodsDataStream
{

public:

    RodsDataInStream(RodsConnection *theConn, RodsObjEntryPtr theObjEntry);

    // Overrides superclass virtual function, open the iRODS data object
    // for reading.
    int openDataObj();

    //
    int getOprInit();

    // Interface for reading from the iRODS data stream to buffer at bufPtr
    // for a block of len bytes.
    int read(void *bufPtr, size_t len);

    //
    int readAdaptive(void *bufPtr, size_t maxLen);

    //
    void getOprEnd();

    //
    const std::string& checksumStr() const;

    //
    const char* checksum() const;

private:

    // we deny copying and the substitution operation
    RodsDataInStream(RodsDataInStream &);
    RodsDataInStream& operator=(RodsDataInStream &);

    // read only to internal buffer
    int read(size_t len);

    // managed pointer to a Kanki object entry
    RodsObjEntryPtr entPtr;

    // data object checksum from rods
    std::string objChecksum;

    // adaptive read size parameter
    size_t adaptiveSize;

    //
    std::vector<std::chrono::milliseconds> dt_;

    //
    std::vector<size_t> bytes_;

    //
    std::vector<long int> speed_;

    //
    std::vector<long int> diff_;
};

} // namespace Kanki


#endif // RODSDATAINSTREAM_H
