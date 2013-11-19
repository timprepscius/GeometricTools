// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.5.0 (2010/10/23)

#ifndef LINEARMERGETREE_H
#define LINEARMERGETREE_H

class LinearMergeTree
{
public:
    // Construction and destruction.  The array is assumed to contain 2^N+1
    // elements where N >= 0.  The application is responsible for any memory
    // management associated with the input array.
    LinearMergeTree (int N);
    ~LinearMergeTree ();

    enum
    {
        CFG_NONE = 0,
        CFG_INCR = 1,
        CFG_DECR = 2,
        CFG_MULT = 3,
        CFG_ROOT_MASK = 3,
        CFG_EDGE = 4,
        CFG_ZERO_SUBEDGE = 8
    };

    bool IsNone (int i) const;
    unsigned char GetRootType (int i) const;
    int GetZeroBase (int i) const;

    void SetEdge (int i);
    bool IsZeroEdge (int i) const;
    bool HasZeroSubedge (int i) const;

    void SetLevel (float level, const float* data, int offset, int stride);

private:
    int mN, mTwoPowerN;
    unsigned char* mNodes;
    short* mZeroBases;
};

#endif

