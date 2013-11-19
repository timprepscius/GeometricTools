// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2012/07/07)

#ifndef VERTEXCOLLAPSE_H
#define VERTEXCOLLAPSE_H

#include "Wm5MinHeap.h"
#include "Wm5Vector3.h"
using namespace Wm5;

class VertexCollapse
{
public:
    // Construction and destruction.
    VertexCollapse (int numVertices, Vector3f*& vertices, bool closed,
        int*& indexMap, int& numEdges, int*& edges);

    ~VertexCollapse ();

protected:
    // Weight calculation for vertex triple <V[m],V[z],V[p]>.
    float GetWeight (int m, int z, int p, Vector3f* vertices);

    // Create data structures for the polyline.
    void CollapseVertices (int numVertices, int* collapses);

    void ComputeEdges (int numVertices, bool closed, int* collapses,
        int* indexMap, int numEdges, int* edges);

    void ReorderVertices (int numVertices, Vector3f*& vertices,
        int* collapses, int numEdges, int* edges);

    // The heap of edge weights.
    MinHeap<int, float>* mHeap;
};

#endif
