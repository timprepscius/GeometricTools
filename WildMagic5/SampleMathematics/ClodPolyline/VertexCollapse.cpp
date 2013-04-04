// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2012/07/07)

#include "VertexCollapse.h"
#include "Wm5DistPoint3Segment3.h"
#include "Wm5Memory.h"

//----------------------------------------------------------------------------
VertexCollapse::VertexCollapse (int numVertices, Vector3f*& vertices,
    bool closed, int*& indexMap, int& numEdges, int*& edges)
{
    indexMap = new1<int>(numVertices);

    if (closed)
    {
        numEdges = numVertices;
        edges = new1<int>(2*numEdges);

        if (numVertices == 3)
        {
            indexMap[0] = 0;
            indexMap[1] = 1;
            indexMap[2] = 3;
            edges[0] = 0;  edges[1] = 1;
            edges[2] = 1;  edges[3] = 2;
            edges[4] = 2;  edges[5] = 0;
            return;
        }
    }
    else
    {
        numEdges = numVertices - 1;
        edges = new1<int>(2*numEdges);

        if (numVertices == 2)
        {
            indexMap[0] = 0;
            indexMap[1] = 1;
            edges[0] = 0;  edges[1] = 1;
            return;
        }
    }

    // Create the heap of weights.
    mHeap = new0 MinHeap<int, float>(numVertices, 0, Mathf::MAX_REAL);
    int qm1 = numVertices - 1;
    if (closed)
    {
        int qm2 = numVertices - 2;
        mHeap->Insert(0, GetWeight(qm1, 0, 1, vertices));
        mHeap->Insert(qm1, GetWeight(qm2, qm1, 0, vertices));
    }
    else
    {
        mHeap->Insert(0, Mathf::MAX_REAL);
        mHeap->Insert(qm1, Mathf::MAX_REAL);
    }
    for (int m = 0, z = 1, p = 2; z < qm1; ++m, ++z, ++p)
    {
        mHeap->Insert(z, GetWeight(m, z, p, vertices));
    }

    // Create the level of detail information for the polyline.
    int* collapses = new1<int>(numVertices);
    CollapseVertices(numVertices, collapses);
    ComputeEdges(numVertices, closed, collapses, indexMap, numEdges, edges);
    ReorderVertices(numVertices, vertices, collapses, numEdges, edges);
    delete1(collapses);
}
//----------------------------------------------------------------------------
VertexCollapse::~VertexCollapse ()
{
    delete0(mHeap);
}
//----------------------------------------------------------------------------
float VertexCollapse::GetWeight (int m, int z, int p, Vector3f* vertices)
{
    Segment3f segment(vertices[m], vertices[p]);
    if (segment.Extent > 0.0f)
    {
        float distance = DistPoint3Segment3f(vertices[z], segment).Get();
        return 0.5f*distance/segment.Extent;
    }
    return Mathf::MAX_REAL;
}
//----------------------------------------------------------------------------
void VertexCollapse::CollapseVertices (int numVertices, int* collapses)
{
    for (int i = numVertices - 1; i >= 0; --i)
    {
        float weight;
        mHeap->Remove(collapses[i], weight);
    }
}
//----------------------------------------------------------------------------
void VertexCollapse::ComputeEdges (int numVertices, bool closed,
    int* collapses, int* indexMap, int numEdges, int* edges)
{
    // Compute the edges (first to collapse is last in array).  Do not
    // collapse last line segment of open polyline.  Do not collapse last
    // triangle of closed polyline.
    int i, vIndex, eIndex = 2*numEdges - 1;
    if (closed)
    {
        for (i = numVertices - 1; i >= 0; --i)
        {
            vIndex = collapses[i];
            edges[eIndex--] = (vIndex + 1) % numVertices;
            edges[eIndex--] = vIndex;
        }
    }
    else
    {
        for (i = numVertices - 1; i >= 2; --i)
        {
            vIndex = collapses[i];
            edges[eIndex--] = vIndex + 1;
            edges[eIndex--] = vIndex;
        }

        vIndex = collapses[0];
        edges[0] = vIndex;
        edges[1] = vIndex + 1;
    }

    // In the given edge order, find the index in the edge array that
    // corresponds to a collapse vertex and save the index for the dynamic
    // change in level of detail.  This relies on the assumption that a
    // vertex is shared by at most two edges.
    eIndex = 2*numEdges - 1;
    for (i = numVertices - 1; i >= 0; --i)
    {
        vIndex = collapses[i];
        for (int e = 0; e < 2*numEdges; ++e)
        {
            if (vIndex == edges[e])
            {
                indexMap[i] = e;
                edges[e] = edges[eIndex];
                break;
            }
        }
        eIndex -= 2;

        if (closed)
        {
            if (eIndex == 5)
            {
                break;
            }
        }
        else
        {
            if (eIndex == 1)
            {
                break;
            }
        }
    }

    // Restore the edge array to full level of detail.
    if (closed)
    {
        for (i = 3; i < numVertices; ++i)
        {
            edges[indexMap[i]] = collapses[i];
        }
    }
    else
    {
        for (i = 2; i < numVertices; ++i)
        {
            edges[indexMap[i]] = collapses[i];
        }
    }
}
//----------------------------------------------------------------------------
void VertexCollapse::ReorderVertices (int numVertices, Vector3f*& vertices,
    int* collapses, int numEdges, int* edges)
{
    int* permute = new1<int>(numVertices);
    Vector3f* permutedVertex = new1<Vector3f>(numVertices);

    int i;
    for (i = 0; i < numVertices; ++i)
    {
        int vIndex = collapses[i];
        permute[vIndex] = i;
        permutedVertex[i] = vertices[vIndex];
    }

    for (i = 0; i < 2*numEdges; ++i)
    {
        edges[i] = permute[edges[i]];
    }

    delete1(permute);
    delete1(vertices);
    vertices = permutedVertex;
}
//----------------------------------------------------------------------------
