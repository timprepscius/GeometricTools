// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "MTITriangle.h"

//----------------------------------------------------------------------------
MTITriangle::MTITriangle (int label0, int label1, int label2)
{
    if (label0 < label1)
    {
        if (label0 < label2)
        {
            // L0 is minimum
            mLabel[0] = label0;
            mLabel[1] = label1;
            mLabel[2] = label2;
        }
        else
        {
            // L2 is minimum
            mLabel[0] = label2;
            mLabel[1] = label0;
            mLabel[2] = label1;
        }
    }
    else
    {
        if (label1 < label2)
        {
            // L1 is minimum
            mLabel[0] = label1;
            mLabel[1] = label2;
            mLabel[2] = label0;
        }
        else
        {
            // L2 is minimum
            mLabel[0] = label2;
            mLabel[1] = label0;
            mLabel[2] = label1;
        }
    }
}
//----------------------------------------------------------------------------
inline int MTITriangle::GetLabel (int i) const
{
    return mLabel[i];
}
//----------------------------------------------------------------------------
bool MTITriangle::operator< (const MTITriangle& triangle) const
{
    if (mLabel[2] < triangle.mLabel[2])
        return true;

    if (mLabel[2] == triangle.mLabel[2])
    {
        if (mLabel[1] < triangle.mLabel[1])
        {
            return true;
        }

        if (mLabel[1] == triangle.mLabel[1])
        {
            return mLabel[0] < triangle.mLabel[0];
        }
    }

    return false;
}
//----------------------------------------------------------------------------
bool MTITriangle::operator== (const MTITriangle& triangle) const
{
    return (mLabel[0] == triangle.mLabel[0]) &&
    ((mLabel[1] == triangle.mLabel[1] && mLabel[2] == triangle.mLabel[2])
    ||
    (mLabel[1] == triangle.mLabel[2] && mLabel[2] == triangle.mLabel[1]));
}
//----------------------------------------------------------------------------
bool MTITriangle::operator!= (const MTITriangle& triangle) const
{
    return !operator==(triangle);
}
//----------------------------------------------------------------------------
