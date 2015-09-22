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

    // Interface for reading from the iRODS data stream to buffer at extBuf
    // for a block of len bytes.
    int read(void *extBuf, size_t len);

private:

    // we deny copying and the substitution operation
    RodsDataInStream(RodsDataInStream &inStream);
    RodsDataInStream& operator=(RodsDataInStream &inStream);

    // managed pointer to a Kanki object entry
    RodsObjEntryPtr entPtr;    
};

} // namespace Kanki


#endif // RODSDATAINSTREAM_H
