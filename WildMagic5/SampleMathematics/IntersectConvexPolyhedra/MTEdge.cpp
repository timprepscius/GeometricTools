// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "MTEdge.h"

//----------------------------------------------------------------------------
MTEdge::MTEdge (int label)
    :
    mLabel(label)
{
    for (int i = 0; i < 2; ++i)
    {
        mVertex[i] = -1;
        mTriangle[i] = -1;
    }
}
//----------------------------------------------------------------------------
MTEdge::MTEdge (const MTEdge& edge)
    :
    mLabel(edge.mLabel)
{
    for (int i = 0; i < 2; ++i)
    {
        mVertex[i] = edge.mVertex[i];
        mTriangle[i] = edge.mTriangle[i];
    }
}
//----------------------------------------------------------------------------
MTEdge::~MTEdge ()
{
}
//----------------------------------------------------------------------------
MTEdge& MTEdge::operator= (const MTEdge& edge)
{
    mLabel = edge.mLabel;
    for (int i = 0; i < 2; ++i)
    {
        mVertex[i] = edge.mVertex[i];
        mTriangle[i] = edge.mTriangle[i];
    }
    return *this;
}
//----------------------------------------------------------------------------
bool MTEdge::ReplaceVertex (int vOld, int vNew)
{
    for (int i = 0; i < 2; ++i)
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
bool MTEdge::ReplaceTriangle (int tOld, int tNew)
{
    for (int i = 0; i < 2; ++i)
    {
        if (mTriangle[i] == tOld)
        {
            mTriangle[i] = tNew;
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
bool MTEdge::operator== (const MTEdge& edge) const
{
    return
        (mVertex[0] == edge.mVertex[0] &&
         mVertex[1] == edge.mVertex[1]) ||
        (mVertex[0] == edge.mVertex[1] &&
         mVertex[1] == edge.mVertex[0]);
}
//----------------------------------------------------------------------------
