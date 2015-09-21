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

    //
    RodsDataInStream(RodsConnection *theConn, RodsObjEntryPtr theObjEntry);

    //
    int openDataObj();

private:

    //
    RodsConnection *connPtr;

    //
    RodsObjEntryPtr entPtr;

    // rods api first class object index (object handle)
    int rodsL1Inx;
};

} // namespace Kanki


#endif // RODSDATAINSTREAM_H
