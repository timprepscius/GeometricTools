// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "MTTriangle.h"

//----------------------------------------------------------------------------
MTTriangle::MTTriangle (int label)
    :
    mLabel(label)
{
    for (int i = 0; i < 3; ++i)
    {
        mVertex[i] = -1;
        mEdge[i] = -1;
        mAdjacent[i] = -1;
    }
}
//----------------------------------------------------------------------------
MTTriangle::MTTriangle (const MTTriangle& triangle)
    :
    mLabel(triangle.mLabel)
{
    for (int i = 0; i < 3; ++i)
    {
        mVertex[i] = triangle.mVertex[i];
        mEdge[i] = triangle.mEdge[i];
        mAdjacent[i] = triangle.mAdjacent[i];
    }
}
//----------------------------------------------------------------------------
MTTriangle::~MTTriangle ()
{
}
//----------------------------------------------------------------------------
MTTriangle& MTTriangle::operator= (const MTTriangle& triangle)
{
    mLabel = triangle.mLabel;
    for (int i = 0; i < 3; ++i)
    {
        mVertex[i] = triangle.mVertex[i];
        mEdge[i] = triangle.mEdge[i];
        mAdjacent[i] = triangle.mAdjacent[i];
    }
    return *this;
}
//----------------------------------------------------------------------------
bool MTTriangle::ReplaceVertex (int vOld, int vNew)
{
    for (int i = 0; i < 3; ++i)
    {
        if (mVertex[i] == vOld)
        {
            mVertex[i] = vNew;
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
bool MTTriangle::ReplaceEdge (int eOld, int eNew)
{
    for (int i = 0; i < 3; ++i)
    {
        if (mEdge[i] == eOld)
        {
            mEdge[i] = eNew;
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
bool MTTriangle::ReplaceAdjacent (int aOld, int aNew)
{
    for (int i = 0; i < 3; ++i)
    {
        if (mAdjacent[i] == aOld)
        {
            mAdjacent[i] = aNew;
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
bool MTTriangle::operator== (const MTTriangle& triangle) const
{
    if (mVertex[0] == triangle.mVertex[0])
    {
        return mVertex[1] == triangle.mVertex[1]
            && mVertex[2] == triangle.mVertex[2];
    }

    if (mVertex[0] == triangle.mVertex[1])
    {
        return mVertex[1] == triangle.mVertex[2]
            && mVertex[2] == triangle.mVertex[0];
    }

    if (mVertex[0] == triangle.mVertex[2])
    {
        return mVertex[1] == triangle.mVertex[0]
            && mVertex[2] == triangle.mVertex[1];
    }

    return false;
}
//----------------------------------------------------------------------------
