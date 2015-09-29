/**
 * @file rodsdatastream.h
 * @brief Definition of Kanki library class RodsDataStream
 *
 * The RodsDataStream class in Kanki provides an abstract interface
 * to the iRODS protocol streaming I/O operations.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

#ifndef RODSDATASTREAM_H
#define RODSDATASTREAM_H

#include <cstdlib>

#include "rodsconnection.h"

#define __KANKI_BUFSIZE_INIT    262144
#define __KANKI_BUFSIZE_INCR    1048576
#define __KANKI_BUFSIZE_MAX     33554432

namespace Kanki {

class RodsDataStream
{

public:

    RodsDataStream(Kanki::RodsConnection *theConn);

    ~RodsDataStream();

    // Pure virtual function to provide an interface to open an iRODS
    // data stream to a data object.
    virtual int openDataObj() = 0;

    // Executes iRODS API seek operation on the data stream for offset
    // bytes from whence.
    int seek(size_t offset, int whence);

    // Closes iRODS data object handle.
    int closeDataObj();

protected:

    // tries to grow internal buffer for requested new size
    size_t growBuffer(size_t newSize);

    // rods connection object pointer
    RodsConnection *connPtr;

    // internal memory buffer for I/O
    void *memBuffer;

    // current internal buffer size
    size_t bufSize;

    // last I/O operation size
    size_t lastOprSize;

    // rods api first class object index (object handle)
    int rodsL1Inx;
};

} // namespace Kanki

#endif // RODSDATASTREAM_H
