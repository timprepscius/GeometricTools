// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.5.0 (2010/10/23)

#include "AreaMergeTree.h"
#include "Wm5Memory.h"
using namespace Wm5;

//----------------------------------------------------------------------------
AreaMergeTree::AreaMergeTree (int N, LinearMergeTree** xMerge,
    LinearMergeTree** yMerge)
    :
    mN(N),
    mQuantity(((1 << 2*(N + 1)) - 1)/3),
    mXMerge(xMerge),
    mYMerge(yMerge)
{
    mNodes = new1<QuadNode>(mQuantity);
}
//----------------------------------------------------------------------------
AreaMergeTree::~AreaMergeTree ()
{
    delete1(mNodes);
}
//----------------------------------------------------------------------------
int AreaMergeTree::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
const QuadNode& AreaMergeTree::GetNode (int i) const
{
    assertion(0 <= i && i < mQuantity, "Invalid index.\n");
    return mNodes[i];
}
//----------------------------------------------------------------------------
void AreaMergeTree::ConstructMono (int A, int LX, int LY, int xOrigin,
    int yOrigin, int stride, int depth)
{
    if (stride > 1)  // internal nodes
    {
        int hStride = stride/2;

        int ABase = 4*A;
        int A00 = ++ABase;
        int A10 = ++ABase;
        int A01 = ++ABase;
        int A11 = ++ABase;

        int LXBase = 2*LX;
        int LX0 = ++LXBase;
        int LX1 = ++LXBase;

        int LYBase = 2*LY;
        int LY0 = ++LYBase;
        int LY1 = ++LYBase;

        int xNext = xOrigin + hStride;
        int yNext = yOrigin + hStride;

        int depthM1 = depth - 1;
        ConstructMono(A00, LX0, LY0, xOrigin, yOrigin, hStride, depthM1);
        ConstructMono(A10, LX1, LY0, xNext, yOrigin, hStride, depthM1);
        ConstructMono(A01, LX0, LY1, xOrigin, yNext, hStride, depthM1);
        ConstructMono(A11, LX1, LY1, xNext, yNext, hStride, depthM1);

        if (depth >= 0)
        {
            // Merging is prevented above the specified depth in the tree.
            // This allows a single object to produce any resolution
            // isocontour rather than using multiple objects to do so.
            mNodes[A].Initialize(xOrigin, yOrigin, xNext, yNext, hStride);
            return;
        }

        bool mono00 = mNodes[A00].IsMono();
        bool mono10 = mNodes[A10].IsMono();
        bool mono01 = mNodes[A01].IsMono();
        bool mono11 = mNodes[A11].IsMono();

        QuadNode node0(xOrigin, yOrigin, xNext, yNext, hStride);
        QuadNode node1 = node0;

        // Merge x first, y second.
        if (mono00 && mono10)
        {
            DoXMerge(node0.mR00, node0.mR10, LX, yOrigin);
        }
        if (mono01 && mono11)
        {
            DoXMerge(node0.mR01, node0.mR11, LX, yNext);
        }
        if (mono00 && mono01)
        {
            DoYMerge(node0.mR00, node0.mR01, xOrigin, LY);
        }
        if (mono10 && mono11)
        {
            DoYMerge(node0.mR10, node0.mR11, xNext, LY);
        }

        // Merge y first, x second.
        if (mono00 && mono01)
        {
            DoYMerge(node1.mR00, node1.mR01, xOrigin, LY);
        }
        if (mono10 && mono11)
        {
            DoYMerge(node1.mR10, node1.mR11, xNext, LY);
        }
        if (mono00 && mono10)
        {
            DoXMerge(node1.mR00, node1.mR10, LX, yOrigin);
        }
        if (mono01 && mono11)
        {
            DoXMerge(node1.mR01, node1.mR11, LX, yNext);
        }

        // Choose the merge that produced the smallest number of rectangles.
        if (node0.GetQuantity() <= node1.GetQuantity())
        {
            mNodes[A] = node0;
        }
        else
        {
            mNodes[A] = node1;
        }
    }
    else  // leaf nodes
    {
        mNodes[A].mR00.Initialize(xOrigin, yOrigin, 1, 1);
    }
}
//----------------------------------------------------------------------------
void AreaMergeTree::DoXMerge (QuadRectangle& r0, QuadRectangle& r1,
    int LX, int yOrigin)
{
    if (!r0.mValid || !r1.mValid)
    {
        return;
    }

    if (r0.mYStride != r1.mYStride)
    {
        return;
    }

    // Rectangles are x-mergeable.
    int incr = 0, decr = 0;
    for (int y = 0; y <= r0.mYStride; ++y)
    {
        switch (mXMerge[yOrigin + y]->GetNode(LX))
        {
        case LinearMergeTree::CFG_MULT:
            return;
        case LinearMergeTree::CFG_INCR:
            ++incr;
            break;
        case LinearMergeTree::CFG_DECR:
            ++decr;
            break;
        }
    }

    if (incr == 0 || decr == 0)
    {
        // Strongly mono, x-merge the rectangles.
        r0.mXStride *= 2;
        r1.mValid = false;
    }
}
//----------------------------------------------------------------------------
void AreaMergeTree::DoYMerge (QuadRectangle& r0, QuadRectangle& r1,
    int xOrigin, int LY)
{
    if (!r0.mValid || !r1.mValid || r0.mXStride != r1.mXStride)
    {
        return;
    }

    // Rectangles are y-mergeable.
    int incr = 0, decr = 0;
    for (int x = 0; x <= r0.mXStride; ++x)
    {
        switch (mYMerge[xOrigin + x]->GetNode(LY))
        {
        case LinearMergeTree::CFG_MULT:
            return;
        case LinearMergeTree::CFG_INCR:
            ++incr;
            break;
        case LinearMergeTree::CFG_DECR:
            ++decr;
            break;
        }
    }

    if (incr == 0 || decr == 0)
    {
        // Strongly mono, y-merge the rectangles.
        r0.mYStride *= 2;
        r1.mValid = false;
    }
}
//----------------------------------------------------------------------------
Rectangle2 AreaMergeTree::GetRectangle (const QuadRectangle& qrect, int LX,
    int LY)
{
    Rectangle2 rect(qrect.mXOrigin, qrect.mYOrigin, qrect.mXStride,
        qrect.mYStride);

    // xmin edge
    LinearMergeTree* merge = mYMerge[qrect.mXOrigin];
    if (merge->GetNode(LY) != LinearMergeTree::CFG_NONE)
    {
        rect.mYofXMin = merge->GetEdge(LY);
        if (rect.mYofXMin != -1)
        {
            rect.mType |= 0x01;
        }
    }

    // xmax edge
    merge = mYMerge[qrect.mXOrigin + qrect.mXStride];
    if (merge->GetNode(LY) != LinearMergeTree::CFG_NONE)
    {
        rect.mYofXMax = merge->GetEdge(LY);
        if (rect.mYofXMax != -1)
        {
            rect.mType |= 0x02;
        }
    }

    // ymin edge
    merge = mXMerge[qrect.mYOrigin];
    if (merge->GetNode(LX) != LinearMergeTree::CFG_NONE)
    {
        rect.mXofYMin = merge->GetEdge(LX);
        if (rect.mXofYMin != -1)
        {
            rect.mType |= 0x04;
        }
    }

    // ymax edge
    merge = mXMerge[qrect.mYOrigin + qrect.mYStride];
    if (merge->GetNode(LX) != LinearMergeTree::CFG_NONE)
    {
        rect.mXofYMax = merge->GetEdge(LX);
        if (rect.mXofYMax != -1)
        {
            rect.mType |= 0x08;
        }
    }

    return rect;
}
//----------------------------------------------------------------------------
void AreaMergeTree::GetRectangles (int A, int LX, int LY, int xOrigin,
    int yOrigin, int stride, std::vector<Rectangle2>& rectangles)
{
    int hStride = stride/2;
    int ABase = 4*A;
    int A00 = ++ABase;
    int A10 = ++ABase;
    int A01 = ++ABase;
    int A11 = ++ABase;
    int LXBase = 2*LX;
    int LX0 = ++LXBase;
    int LX1 = ++LXBase;
    int LYBase = 2*LY;
    int LY0 = ++LYBase;
    int LY1 = ++LYBase;
    int xNext = xOrigin + hStride;
    int yNext = yOrigin + hStride;

    const QuadRectangle& r00 = mNodes[A].mR00;
    if (r00.mValid)
    {
        if (r00.mXStride == stride)
        {
            if (r00.mYStride == stride)
            {
                rectangles.push_back(GetRectangle(r00, LX, LY));
            }
            else
            {
                rectangles.push_back(GetRectangle(r00, LX, LY0));
            }
        }
        else
        {
            if (r00.mYStride == stride)
            {
                rectangles.push_back(GetRectangle(r00, LX0, LY));
            }
            else
            {
                GetRectangles(A00, LX0, LY0, xOrigin, yOrigin, hStride,
                    rectangles);
            }
        }
    }

    const QuadRectangle& r10 = mNodes[A].mR10;
    if (r10.mValid)
    {
        if (r10.mYStride == stride)
        {
            rectangles.push_back(GetRectangle(r10, LX1, LY));
        }
        else
        {
            GetRectangles(A10, LX1, LY0, xNext, yOrigin, hStride,
                rectangles);
        }
    }

    const QuadRectangle& r01 = mNodes[A].mR01;
    if (r01.mValid)
    {
        if (r01.mXStride == stride)
        {
            rectangles.push_back(GetRectangle(r01, LX, LY1));
        }
        else
        {
            GetRectangles(A01, LX0, LY1, xOrigin, yNext, hStride,
                rectangles);
        }
    }

    const QuadRectangle& r11 = mNodes[A].mR11;
    if (r11.mValid)
    {
        GetRectangles(A11, LX1, LY1, xNext, yNext, hStride, rectangles);
    }
}
//----------------------------------------------------------------------------
