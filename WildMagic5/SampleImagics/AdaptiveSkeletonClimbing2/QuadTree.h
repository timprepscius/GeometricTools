// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.5.0 (2010/10/23)

#ifndef QUADTREE_H
#define QUADTREE_H

class QuadRectangle
{
public:
    // Construction and destruction.
    QuadRectangle ();
    QuadRectangle (int xOrigin, int yOrigin, int xStride, int yStride);
    ~QuadRectangle ();

    void Initialize (int xOrigin, int yOrigin, int xStride, int yStride);

    int mXOrigin, mYOrigin, mXStride, mYStride;
    bool mValid;
};

class QuadNode
{
public:
    // Construction and destruction.
    QuadNode ();
    QuadNode (int xOrigin, int yOrigin, int xNext, int yNext, int stride);
    ~QuadNode ();

    void Initialize (int xOrigin, int yOrigin, int xNext, int yNext,
        int stride);

    bool IsMono () const;
    int GetQuantity () const;

    QuadRectangle mR00, mR10, mR01, mR11;
};

#endif

