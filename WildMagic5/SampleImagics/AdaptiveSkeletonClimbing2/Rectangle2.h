// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.5.0 (2010/10/23)

#ifndef RECTANGLE2_H
#define RECTANGLE2_H

class Rectangle2
{
public:
    // Construction and destruction.
    Rectangle2 (int xOrigin, int yOrigin, int xStride, int yStride);
    ~Rectangle2 ();

    int mXOrigin, mYOrigin, mXStride, mYStride;
    int mYofXMin, mYofXMax, mXofYMin, mXofYMax;

    // A 4-bit flag for how the level set intersects the rectangle boundary.
    //   bit 0 = xmin edge
    //   bit 1 = xmax edge
    //   bit 2 = ymin edge
    //   bit 3 = ymax edge
    // A bit is set if the corresponding edge is intersected by the level set.
    // This information is known from the CFG flags for LinearMergeTree.
    // Intersection occurs whenever the flag is CFG_INCR or CFG_DECR.
    unsigned char mType;
};

#endif
