// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.5.0 (2010/10/23)

#ifndef AREAMERGETREE_H
#define AREAMERGETREE_H

#include "LinearMergeTree.h"
#include "QuadTree.h"
#include "Rectangle2.h"
#include <vector>

class AreaMergeTree
{
public:
    // Construction and destruction.
    AreaMergeTree (int N, LinearMergeTree** xMerge,
        LinearMergeTree** yMerge);
    ~AreaMergeTree ();

    // Member access.
    int GetQuantity () const;
    const QuadNode& GetNode (int i) const;

    void ConstructMono (int A, int LX, int LY, int xOrigin, int yOrigin,
        int stride, int depth);

    void GetRectangles (int A, int LX, int LY, int xOrigin, int yOrigin,
        int xtride, std::vector<Rectangle2>& rectangles);

private:
    void DoXMerge (QuadRectangle& r0, QuadRectangle& r1, int ellX,
        int yOrigin);

    void DoYMerge (QuadRectangle& r0, QuadRectangle& r1, int xOrigin,
        int ellY);

    Rectangle2 GetRectangle (const QuadRectangle& qrect, int ellX, int ellY);

    int mN, mQuantity;
    LinearMergeTree** mXMerge;
    LinearMergeTree** mYMerge;
    QuadNode* mNodes;
};

#endif

