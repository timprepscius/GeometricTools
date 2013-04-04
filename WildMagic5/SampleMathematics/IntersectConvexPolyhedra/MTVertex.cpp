// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "MTVertex.h"

//----------------------------------------------------------------------------
MTVertex::MTVertex (int label, int eGrow, int tGrow)
    :
    mLabel(label),
    mESet(eGrow, eGrow),
    mTSet(tGrow, tGrow)
{
}
//----------------------------------------------------------------------------
MTVertex::MTVertex (const MTVertex& vertex)
    :
    mLabel(vertex.mLabel),
    mESet(vertex.mESet),
    mTSet(vertex.mTSet)
{
}
//----------------------------------------------------------------------------
MTVertex::~MTVertex ()
{
}
//----------------------------------------------------------------------------
MTVertex& MTVertex::operator= (const MTVertex& vertex)
{
    mLabel = vertex.mLabel;
    mESet = vertex.mESet;
    mTSet = vertex.mTSet;
    return *this;
}
//----------------------------------------------------------------------------
bool MTVertex::ReplaceEdge (int eOld, int eNew)
{
    const int numElements = mESet.GetNumElements();
    for (int i = 0; i < numElements; ++i)
    {
        if (mESet[i] == eOld)
        {
            mESet[i] = eNew;
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
bool MTVertex::ReplaceTriangle (int tOld, int tNew)
{
    const int numTriangles = mTSet.GetNumElements();
    for (int i = 0; i < numTriangles; ++i)
    {
        if (mTSet[i] == tOld)
        {
            mTSet[i] = tNew;
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
