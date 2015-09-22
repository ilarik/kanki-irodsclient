/**
 * @file rodsdataoutstream.cpp
 * @brief Implementation of Kanki library class RodsDataOutStream
 *
 * The RodsDataOutStream class in Kanki provides an interface to the
 * iRODS protocol streaming write operations.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

// Kanki library class RodsDataOutStream header
#include "rodsdataoutstream.h"

namespace Kanki {

RodsDataOutStream::RodsDataOutStream(RodsConnection *theConn)
    : RodsDataStream(theConn)
{

}

int RodsDataOutStream::openDataObj()
{
    return (0);
}

} // namespace Kanki
