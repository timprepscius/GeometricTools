// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.5.0 (2010/10/23)

#include "QuadTree.h"

//----------------------------------------------------------------------------
// QuadRectangle
//----------------------------------------------------------------------------
QuadRectangle::QuadRectangle ()
    :
    mXOrigin(0),
    mYOrigin(0),
    mXStride(0),
    mYStride(0),
    mValid(false)
{
    mValid = false;
}
//----------------------------------------------------------------------------
QuadRectangle::QuadRectangle (int xOrigin, int yOrigin, int xStride,
    int yStride)
{
    Initialize(xOrigin, yOrigin, xStride, yStride);
}
//----------------------------------------------------------------------------
QuadRectangle::~QuadRectangle ()
{
}
//----------------------------------------------------------------------------
void QuadRectangle::Initialize (int xOrigin, int yOrigin, int xStride,
    int yStride)
{
    mXOrigin = xOrigin;
    mYOrigin = yOrigin;
    mXStride = xStride;
    mYStride = yStride;
    mValid = true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// QuadNode
//----------------------------------------------------------------------------
QuadNode::QuadNode ()
{
}
//----------------------------------------------------------------------------
QuadNode::QuadNode (int xOrigin, int yOrigin, int xNext, int yNext,
    int stride)
    :
    mR00(xOrigin, yOrigin, stride, stride),
    mR10(xNext, yOrigin, stride, stride),
    mR01(xOrigin, yNext, stride, stride),
    mR11(xNext, yNext, stride, stride)
{
}
//----------------------------------------------------------------------------
QuadNode::~QuadNode ()
{
}
//----------------------------------------------------------------------------
void QuadNode::Initialize (int xOrigin, int yOrigin, int xNext, int yNext,
    int stride)
{
    mR00.Initialize(xOrigin, yOrigin, stride, stride);
    mR10.Initialize(xNext, yOrigin, stride, stride);
    mR01.Initialize(xOrigin, yNext, stride, stride);
    mR11.Initialize(xNext, yNext, stride, stride);
}
//----------------------------------------------------------------------------
bool QuadNode::IsMono () const
{
    return !mR10.mValid && !mR01.mValid && !mR11.mValid;
}
//----------------------------------------------------------------------------
int QuadNode::GetQuantity () const
{
    int quantity = 0;

    if (mR00.mValid)
    {
        ++quantity;
    }

    if (mR10.mValid)
    {
        ++quantity;
    }

    if (mR01.mValid)
    {
        ++quantity;
    }

    if (mR11.mValid)
    {
        ++quantity;
    }

    return quantity;
}
//----------------------------------------------------------------------------

