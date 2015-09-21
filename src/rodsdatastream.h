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

#define __KANKI_INIT_BUFSIZE    4096

namespace Kanki {

class RodsDataStream
{

public:

    RodsDataStream();
    ~RodsDataStream();

protected:

    void *memBuffer;
    unsigned int bufSize;
};

} // namespace Kanki

#endif // RODSDATASTREAM_H
