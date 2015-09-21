/**
 * @file rodsdatainstream.cpp
 * @brief Implementation of Kanki library class RodsDataInStream
 *
 * The RodsDataInStream class in Kanki provides an interface to the
 * iRODS protocol streaming read operations.
 *
 * Copyright (C) 2014-2015 University of Jyväskylä. All rights reserved.
 * License: The BSD 3-Clause License, see LICENSE file for details.
 *
 * @author Ilari Korhonen
 */

// Kanki library class RodsDataInStream header
#include "rodsdatainstream.h"

namespace Kanki {

RodsDataInStream::RodsDataInStream(RodsConnection *theConn, RodsObjEntryPtr theObjEntry)
    : RodsDataStream()
{
    this->connPtr = theConn;
    this->entPtr = theObjEntry;
}

int RodsDataInStream::openDataObj()
{
    dataObjInp_t openParam;
    int openResult = 0;

    memset(&openParam, 0, sizeof (openParam));

    openParam.openFlags = O_RDONLY;
    rstrcpy(openParam.objPath, this->entPtr->getObjectFullPath().c_str(), MAX_NAME_LEN);

    if ((openResult = rcDataObjOpen(this->connPtr->commPtr(), &openParam)) < 0)
        return (openResult);

    else
        this->rodsL1Inx = openResult;

    return (0);
}

} // namespace Kanki
