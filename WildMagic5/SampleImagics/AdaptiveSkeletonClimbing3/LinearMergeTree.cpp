// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.5.1 (2012/07/06)

#include "LinearMergeTree.h"
#include "Wm5Memory.h"
using namespace Wm5;

//----------------------------------------------------------------------------
LinearMergeTree::LinearMergeTree (int N)
    :
    mN(N),
    mTwoPowerN(1 << N)
{
    int numElements = 2*mTwoPowerN - 1;
    mNodes = new1<unsigned char>(numElements);
    mZeroBases = new1<short>(numElements);
}
//----------------------------------------------------------------------------
LinearMergeTree::~LinearMergeTree ()
{
    delete1(mNodes);
    delete1(mZeroBases);
}
//----------------------------------------------------------------------------
bool LinearMergeTree::IsNone (int i) const
{
    return (mNodes[i] & CFG_ROOT_MASK) == CFG_NONE;
}
//----------------------------------------------------------------------------
unsigned char LinearMergeTree::GetRootType (int i) const
{
    return mNodes[i] & CFG_ROOT_MASK;
}
//----------------------------------------------------------------------------
bool LinearMergeTree::IsZeroEdge (int i) const
{
    return mNodes[i] == (CFG_EDGE | CFG_INCR)
        || mNodes[i] == (CFG_EDGE | CFG_DECR);
}
//----------------------------------------------------------------------------
bool LinearMergeTree::HasZeroSubedge (int i) const
{
    return (mNodes[i] & CFG_ZERO_SUBEDGE) != 0;
}
//----------------------------------------------------------------------------
int LinearMergeTree::GetZeroBase (int i) const
{
    return (int) mZeroBases[i];
}
//----------------------------------------------------------------------------
void LinearMergeTree::SetLevel (float level, const float* data, int offset,
    int stride)
{
    // assert:  level is not an image value

    int firstLeaf = mTwoPowerN - 1;
    for (int i = 0, leaf = firstLeaf; i < mTwoPowerN; ++i, ++leaf)
    {
        int base = offset + stride*i;
        float value0 = data[base];
        float value1 = data[base + stride];

        if (value0 > level)
        {
            if (value1 > level)
            {
                mNodes[leaf] = CFG_NONE;
                mZeroBases[leaf] = -1;
            }
            else
            {
                mNodes[leaf] = CFG_DECR;
                mZeroBases[leaf] = (short)i;
            }
        }
        else  // value0 < level
        {
            if (value1 > level)
            {
                mNodes[leaf] = CFG_INCR;
                mZeroBases[leaf] = (short)i;
            }
            else
            {
                mNodes[leaf] = CFG_NONE;
                mZeroBases[leaf] = -1;
            }
        }
    }

    for (int i = firstLeaf - 1; i >= 0; --i)
    {
        int twoIp1 = 2*i + 1, twoIp2 = twoIp1 + 1;
        unsigned char value0 = mNodes[twoIp1];
        unsigned char value1 = mNodes[twoIp2];
        unsigned char combine = (value0 | value1);
        mNodes[i] = combine;
        if (combine == CFG_INCR)
        {
            if (value0 == CFG_INCR)
            {
                mZeroBases[i] = mZeroBases[twoIp1];
            }
            else
            {
                mZeroBases[i] = mZeroBases[twoIp2];
            }
        }
        else if (combine == CFG_DECR)
        {
            if (value0 == CFG_DECR)
            {
                mZeroBases[i] = mZeroBases[twoIp1];
            }
            else
            {
                mZeroBases[i] = mZeroBases[twoIp2];
            }
        }
        else
        {
            mZeroBases[i] = -1;
        }
    }
}
//----------------------------------------------------------------------------
void LinearMergeTree::SetEdge (int i)
{
    mNodes[i] |= CFG_EDGE;

    // Inform all predecessors whether they have a zero subedge.
    if (mZeroBases[i] >= 0)
    {
        while (i > 0)
        {
            i = (i - 1)/2;
            mNodes[i] |= CFG_ZERO_SUBEDGE;
        }
    }
}
//----------------------------------------------------------------------------

