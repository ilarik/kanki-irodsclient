/**
 * @file rodsdataoutstream.h
 * @brief Definition of Kanki library class RodsDataOutStream
 *
 * The RodsDataOutStream class in Kanki provides an interface to the
 * iRODS protocol streaming write operations.
 *
 * Copyright (C) 2016-2020 KTH Royal Institute of Technology. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * Copyright (C) 2014-2016 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

#ifndef RODSDATAOUTSTREAM_H
#define RODSDATAOUTSTREAM_H

// Kanki iRODS C++ class library headers
#include "rodssession.h"
#include "rodsdatastream.h"

namespace Kanki {

class RodsDataOutStream : public RodsDataStream
{

public:

    RodsDataOutStream(Kanki::RodsSession *theSession);

    // Overrides superclass pure virtual function, opens the iRODS data
    // object for writing.
    int openDataObj();

    // we deny copying and substitution
    RodsDataOutStream(RodsDataOutStream &) = delete;
    RodsDataOutStream& operator=(RodsDataOutStream &) = delete;

private:

};

} // namespace Kanki

#endif // RODSDATAOUTSTREAM_H
