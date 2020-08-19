/**
 * @file rodsdatainstream.h
 * @brief Definition of Kanki library class RodsDataInStream
 *
 * The RodsDataInStream class in Kanki provides an interface to the
 * iRODS protocol streaming read operations.
 *
 * Copyright (C) 2016-2020 KTH Royal Institute of Technology. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * Copyright (C) 2014-2016 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

#ifndef RODSDATAINSTREAM_H
#define RODSDATAINSTREAM_H

// C++ standard library headers
#include <chrono>

// Kanki iRODS C++ class library headers
#include "rodssession.h"
#include "rodsobjentry.h"
#include "rodsdatastream.h"

// interval of last I/O operations to keep track of (adaptive)
#define _KANKI_ADAPTIVE_INT    10

namespace Kanki {

class RodsDataInStream : public RodsDataStream
{

public:

    RodsDataInStream(RodsSession *theSession, RodsObjEntryPtr theObjEntry);
    ~RodsDataInStream();

    // Overrides superclass virtual function, open the iRODS data object
    // for reading.
    int openDataObj();

    // Initiate iRODS API get operation for the data object to which the
    // data stream is associated with.
    int getOprInit();

    // Interface for reading from the iRODS data stream to buffer at bufPtr
    // for a block of len bytes.
    int read(void *bufPtr, size_t len);

    // Adaptive read wrapper to select read operation size depending on
    // history of development of the transfer rate.
    int readAdaptive(void *bufPtr, size_t maxLen);

    // End iRODS API get operation to signal iRODS that GET_OPR is completed.
    void getOprEnd();

    // Interface to inquire parallel tranfer capabilities
    bool parallelXferPortalAvail() const;

    // Interface for accessing the data object checksum string provided
    // by the iRODS server.
    const std::string& checksumStr() const;

    // Interface for accessing the bare C string of the checksum string data.
    const char* checksum() const;

    // we deny copying and the substitution operation
    RodsDataInStream(RodsDataInStream &) = delete;
    RodsDataInStream& operator=(RodsDataInStream &) = delete;

private:

    // read only to internal buffer
    int read(size_t len);

    // managed pointer to a Kanki object entry
    RodsObjEntryPtr entPtr;

    // data object checksum from rods
    std::string objChecksum;

    // adaptive read size parameter
    size_t adaptiveSize;

    // parallel transfer portal parameters
    portalOprOut_t *portalParams;

    // container for the last measured I/O operation durations
    std::vector<std::chrono::milliseconds> dt_;

    // container for the last I/O operation sizes
    std::vector<size_t> bytes_;

    // container for the last measured I/O throughputs
    std::vector<long int> speed_;

    // container for the last computed differences in I/O throughputs
    std::vector<long int> diff_;
};

} // namespace Kanki


#endif // RODSDATAINSTREAM_H
