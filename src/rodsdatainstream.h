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

#include "rodsconnection.h"
#include "rodsobjentry.h"
#include "rodsdatastream.h"

namespace Kanki {

class RodsDataInStream : public RodsDataStream
{

public:

    RodsDataInStream(RodsConnection *theConn, RodsObjEntryPtr theObjEntry);

    // Overrides superclass virtual function, open the iRODS data object
    // for reading.
    int openDataObj();

    //
    int initGetOpr();

    // Interface for reading from the iRODS data stream to buffer at bufPtr
    // for a block of len bytes.
    int read(void *bufPtr, size_t len);

    //
    void getOprDone();

    //
    const std::string& checksumStr() const;

    //
    const char* checksum() const;

private:

    // we deny copying and the substitution operation
    RodsDataInStream(RodsDataInStream &inStream);
    RodsDataInStream& operator=(RodsDataInStream &inStream);

    //
    int read(size_t len);

    // managed pointer to a Kanki object entry
    RodsObjEntryPtr entPtr;

    //
    std::string objChecksum;
};

} // namespace Kanki


#endif // RODSDATAINSTREAM_H