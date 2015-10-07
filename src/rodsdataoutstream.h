/**
 * @file rodsdataoutstream.h
 * @brief Definition of Kanki library class RodsDataOutStream
 *
 * The RodsDataOutStream class in Kanki provides an interface to the
 * iRODS protocol streaming write operations.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

#ifndef RODSDATAOUTSTREAM_H
#define RODSDATAOUTSTREAM_H

// Kanki iRODS C++ class library headers
#include "rodsconnection.h"
#include "rodsdatastream.h"

namespace Kanki {

class RodsDataOutStream : public RodsDataStream
{

public:

    RodsDataOutStream(Kanki::RodsConnection *theConn);

    // Overrides superclass pure virtual function, opens the iRODS data
    // object for writing.
    int openDataObj();

private:

    // we deny copying and substitution
    RodsDataOutStream(RodsDataOutStream &);
    RodsDataOutStream& operator=(RodsDataOutStream &);
};

} // namespace Kanki

#endif // RODSDATAOUTSTREAM_H
