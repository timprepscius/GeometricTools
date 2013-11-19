// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.5.0 (2010/10/23)

#ifndef CLIMB2D_H
#define CLIMB2D_H

#include "AreaMergeTree.h"
#include "Wm5Vector2.h"
using namespace Wm5;

class Climb2D
{
public:
    // Construction and destruction.  The array is assumed to contain 2^N+1 by
    // 2^N+1 elements where N >= 0.  The organization is row-major order.  The
    // class assumes responsibility for the input array and will delete it.
    Climb2D (int N, int* data);
    ~Climb2D ();

    typedef std::pair<int,int> Edge2;

    void ExtractContour (float level, int depth, int& numVertices,
        Vector2f*& vertices, int& numEdges, Edge2*& edges);

    void MakeUnique (int& numVertices, Vector2f*& vertices,
        int& numEdges, Edge2*& edges);

private:
    // image data
    int mN, mTwoPowerN, mSize;
    int* mData;

    // linear merging
    LinearMergeTree** mXMerge;
    LinearMergeTree** mYMerge;

    // area merging
    AreaMergeTree* mXYMerge;

    // Support for extraction of level sets.
    float GetInterp (float level, int base, int index, int increment);
    void AddVertex (std::vector<Vector2f>& vertices, float x, float y);
    void AddEdge (std::vector<Vector2f>& vertices,
        std::vector<Edge2>& edges, float x0, float y0, float x1, float y1);

    void SetLevel (float level, int depth);
    void GetRectangles (std::vector<Rectangle2>& rectangles);
    void GetComponents (float level, const Rectangle2& rectangle,
        std::vector<Vector2f>& vertices, std::vector<Edge2>& edges);

    // debugging
    void PrintRectangles (const char* filename,
        std::vector<Rectangle2>& rectangles);
};

#endif

