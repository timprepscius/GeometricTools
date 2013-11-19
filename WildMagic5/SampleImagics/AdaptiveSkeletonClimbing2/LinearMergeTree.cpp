// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.5.0 (2010/10/23)

#include "LinearMergeTree.h"
#include "Wm5Memory.h"
using namespace Wm5;

//----------------------------------------------------------------------------
LinearMergeTree::LinearMergeTree (int N)
    :
    mN(N),
    mTwoPowerN(1 << N)
{
    mNodes = new1<unsigned char>(2*mTwoPowerN - 1);
}
//----------------------------------------------------------------------------
LinearMergeTree::~LinearMergeTree ()
{
    delete1(mNodes);
}
//----------------------------------------------------------------------------
int LinearMergeTree::GetQuantity () const
{
    return 2*mTwoPowerN - 1;
}
//----------------------------------------------------------------------------
int LinearMergeTree::GetNode (int i) const
{
    return mNodes[i];
}
//----------------------------------------------------------------------------
int LinearMergeTree::GetEdge (int i) const
{
    // assert: mNodes[i] == CFG_INCR || mNodes[i] == CFG_DECR

    // Traverse binary tree looking for incr or decr leaf node.
    int firstLeaf = mTwoPowerN - 1;
    while (i < firstLeaf)
    {
        i = 2*i + 1;
        if (mNodes[i] == CFG_NONE)
        {
            ++i;
        }
    }

    return i - firstLeaf;
}
//----------------------------------------------------------------------------
void LinearMergeTree::SetLevel (float level, const int* data, int offset,
    int stride)
{
    // assert:  level is not an image value

    // Determine the sign changes between pairs of consecutive samples.
    int firstLeaf = mTwoPowerN - 1;
    for (int i = 0, leaf = firstLeaf; i < mTwoPowerN; ++i, ++leaf)
    {
        int base = offset + stride*i;
        float value0 = (float)data[base];
        float value1 = (float)data[base + stride];

        if (value0 > level)
        {
            if (value1 > level)
            {
                mNodes[leaf] = CFG_NONE;
            }
            else
            {
                mNodes[leaf] = CFG_DECR;
            }
        }
        else // value0 < level
        {
            if (value1 > level)
            {
                mNodes[leaf] = CFG_INCR;
            }
            else
            {
                mNodes[leaf] = CFG_NONE;
            }
        }
    }

    // Propagate the sign change information up the binary tree.
    for (int i = firstLeaf - 1; i >= 0; --i)
    {
        int twoIp1 = 2*i + 1;
        int child0 = (int)mNodes[twoIp1];
        int child1 = (int)mNodes[twoIp1 + 1];
        mNodes[i] = (unsigned char)(child0 | child1);
    }
}
//----------------------------------------------------------------------------
