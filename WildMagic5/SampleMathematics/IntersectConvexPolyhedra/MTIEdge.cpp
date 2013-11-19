// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "MTIEdge.h"

//----------------------------------------------------------------------------
MTIEdge::MTIEdge (int label0, int label1)
{
    if (label0 < label1)
    {
        // L0 is minimum
        mLabel[0] = label0;
        mLabel[1] = label1;
    }
    else
    {
        // L1 is minimum
        mLabel[0] = label1;
        mLabel[1] = label0;
    }
}
//----------------------------------------------------------------------------
int MTIEdge::GetLabel (int i) const
{
    return mLabel[i];
}
//----------------------------------------------------------------------------
bool MTIEdge::operator< (const MTIEdge& edge) const
{
    if (mLabel[1] < edge.mLabel[1])
    {
        return true;
    }

    if (mLabel[1] == edge.mLabel[1])
    {
        return mLabel[0] < edge.mLabel[0];
    }

    return false;
}
//----------------------------------------------------------------------------
bool MTIEdge::operator== (const MTIEdge& edge) const
{
    return mLabel[0] == edge.mLabel[0] && mLabel[1] == edge.mLabel[1];
}
//----------------------------------------------------------------------------
bool MTIEdge::operator!= (const MTIEdge& edge) const
{
    return !operator==(edge);
}
//----------------------------------------------------------------------------
