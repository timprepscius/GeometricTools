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
        CFG_NONE,
        CFG_INCR,
        CFG_DECR,
        CFG_MULT
    };

    // Member access.
    int GetQuantity () const;
    int GetNode (int i) const;
    int GetEdge (int i) const;

    void SetLevel (float level, const int* data, int offset, int stride);

private:
    int mN, mTwoPowerN;
    unsigned char* mNodes;
};

#endif

