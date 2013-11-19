// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.5.0 (2010/10/23)

#include "Rectangle2.h"

//----------------------------------------------------------------------------
Rectangle2::Rectangle2 (int xOrigin, int yOrigin, int xStride, int yStride)
    :
    mXOrigin(xOrigin),
    mYOrigin(yOrigin),
    mXStride(xStride),
    mYStride(yStride),
    mYofXMin(-1),
    mYofXMax(-1),
    mXofYMin(-1),
    mXofYMax(-1),
    mType(0)
{
}
//----------------------------------------------------------------------------
Rectangle2::~Rectangle2 ()
{
}
//----------------------------------------------------------------------------
