/**
 * @file rodsdatastream.cpp
 * @brief Implementation of Kanki library class RodsDataStream
 *
 * The RodsDataStream class in Kanki provides an abstract interface
 * to the iRODS protocol streaming I/O operations.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

// Kanki library class RodsDataStream header
#include "rodsdatastream.h"

namespace Kanki {

RodsDataStream::RodsDataStream()
{
    this->memBuffer = std::malloc(__KANKI_INIT_BUFSIZE);
}

RodsDataStream::~RodsDataStream()
{
    free(this->memBuffer);
}

} // namespace Kanki
