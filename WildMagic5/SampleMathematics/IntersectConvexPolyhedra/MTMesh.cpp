// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "MTMesh.h"

//----------------------------------------------------------------------------
MTMesh::MTMesh (int numVertices, int numEdges, int numTriangles)
    :
    mVertices(numVertices),
    mEdges(numEdges),
    mTriangles(numTriangles),
    mInitialELabel(-1),
    mInitialTLabel(-1)
{
}
//----------------------------------------------------------------------------
MTMesh::MTMesh (const MTMesh& mesh)
{
    *this = mesh;
}
//----------------------------------------------------------------------------
MTMesh::~MTMesh ()
{
}
//----------------------------------------------------------------------------
void MTMesh::Reset (int numVertices, int numEdges, int numTriangles)
{
    mVertices.Reset(numVertices);
    mEdges.Reset(numEdges);
    mTriangles.Reset(numTriangles);
    mVMap.clear();
    mEMap.clear();
    mTMap.clear();
}
//----------------------------------------------------------------------------
MTMesh& MTMesh::operator= (const MTMesh& mesh)
{
    mVertices = mesh.mVertices;
    mEdges = mesh.mEdges;
    mTriangles = mesh.mTriangles;
    mVMap = mesh.mVMap;
    mEMap = mesh.mEMap;
    mTMap = mesh.mTMap;
    mInitialELabel = mesh.mInitialELabel;
    mInitialTLabel = mesh.mInitialTLabel;
    return *this;
}
//----------------------------------------------------------------------------
bool MTMesh::Insert (int label0, int label1, int label2)
{
    // Insert the triangle.
    int t = InsertTriangle(label0, label1, label2);
    if (t == -1)
    {
        // The triangle already exists.
        return true;
    }

    // Insert the vertices of the triangle.
    int v0 = InsertVertex(label0);
    int v1 = InsertVertex(label1);
    int v2 = InsertVertex(label2);

    // Insert the edges of the triangle.
    int e0 = InsertEdge(label0, label1);
    int e1 = InsertEdge(label1, label2);
    int e2 = InsertEdge(label2, label0);

    // Set the connections among the components.
    MTTriangle& triangle = mTriangles[t];
    MTVertex& vertex0 = mVertices[v0];
    MTVertex& vertex1 = mVertices[v1];
    MTVertex& vertex2 = mVertices[v2];
    MTEdge& edge0 = mEdges[e0];
    MTEdge& edge1 = mEdges[e1];
    MTEdge& edge2 = mEdges[e2];

    // Attach edges to vertices.
    vertex0.InsertEdge(e2);
    vertex0.InsertEdge(e0);
    vertex1.InsertEdge(e0);
    vertex1.InsertEdge(e1);
    vertex2.InsertEdge(e1);
    vertex2.InsertEdge(e2);
    edge0.Vertex(0) = v0;
    edge0.Vertex(1) = v1;
    edge1.Vertex(0) = v1;
    edge1.Vertex(1) = v2;
    edge2.Vertex(0) = v2;
    edge2.Vertex(1) = v0;

    // Attach triangles to vertices.
    vertex0.InsertTriangle(t);
    vertex1.InsertTriangle(t);
    vertex2.InsertTriangle(t);
    triangle.Vertex(0) = v0;
    triangle.Vertex(1) = v1;
    triangle.Vertex(2) = v2;

    // Attach triangle to edges.
    AttachTriangleToEdge(t, triangle, 0, e0, edge0);
    AttachTriangleToEdge(t, triangle, 1, e1, edge1);
    AttachTriangleToEdge(t, triangle, 2, e2, edge2);

    return true;
}
//----------------------------------------------------------------------------
int MTMesh::InsertVertex (int label)
{
    MTIVertex vertex(label);
    int v;

    VIter iter = mVMap.find(vertex);
    if (iter != mVMap.end())
    {
        // The vertex already exists.
        v = iter->second;
    }
    else
    {
        // Create a new vertex.
        v = mVertices.Append(MTVertex(label));
        mVMap.insert(std::make_pair(vertex, v));
    }

    return v;
}
//----------------------------------------------------------------------------
int MTMesh::InsertEdge (int label0, int label1)
{
    MTIEdge edge(label0, label1);
    int e;

    EIter iter = mEMap.find(edge);
    if (iter != mEMap.end())
    {
        // The edge already exists.
        e = iter->second;
    }
    else
    {
        // Create a new edge.
        e = mEdges.Append(MTEdge(mInitialELabel));
        mEMap.insert(std::make_pair(edge, e));
    }

    return e;
}
//----------------------------------------------------------------------------
int MTMesh::InsertTriangle (int label0, int label1, int label2)
{
    MTITriangle triangle(label0, label1, label2);
    int t;

    TIter iter = mTMap.find(triangle);
    if (iter != mTMap.end())
    {
        // The triangle already exists.
        t = -1;
    }
    else
    {
        // create new triangle
        t = mTriangles.Append(MTTriangle(mInitialTLabel));
        mTMap.insert(std::make_pair(triangle, t));
    }

    return t;
}
//----------------------------------------------------------------------------
bool MTMesh::Remove (int label0, int label1, int label2)
{
    TIter iter = mTMap.find(MTITriangle(label0, label1, label2));
    if (iter == mTMap.end())
    {
        // The triangle does not exist.
        return false;
    }
    int t = iter->second;

    MTTriangle& triangle = mTriangles[t];

    // Detach triangle from edges.
    int e0 = triangle.Edge(0);
    int e1 = triangle.Edge(1);
    int e2 = triangle.Edge(2);
    MTEdge& edge0 = mEdges[e0];
    MTEdge& edge1 = mEdges[e1];
    MTEdge& edge2 = mEdges[e2];
    DetachTriangleFromEdge(t, triangle, 0, e0, edge0);
    DetachTriangleFromEdge(t, triangle, 1, e1, edge1);
    DetachTriangleFromEdge(t, triangle, 2, e2, edge2);

    // Detach triangle from vertices.
    int v0 = triangle.Vertex(0);
    MTVertex& vertex0 = mVertices[v0];
    vertex0.RemoveTriangle(t);

    int v1 = triangle.Vertex(1);
    MTVertex& vertex1 = mVertices[v1];
    vertex1.RemoveTriangle(t);

    int v2 = triangle.Vertex(2);
    MTVertex& vertex2 = mVertices[v2];
    vertex2.RemoveTriangle(t);

    // Detach edges from vertices (only if last edge to reference vertex).
    bool e0Destroy = (edge0.Triangle(0) == -1);
    if (e0Destroy)
    {
        vertex0.RemoveEdge(e0);
        vertex1.RemoveEdge(e0);
    }

    bool e1Destroy = (edge1.Triangle(0) == -1);
    if (e1Destroy)
    {
        vertex1.RemoveEdge(e1);
        vertex2.RemoveEdge(e1);
    }

    bool e2Destroy = (edge2.Triangle(0) == -1);
    if (e2Destroy)
    {
        vertex0.RemoveEdge(e2);
        vertex2.RemoveEdge(e2);
    }

    // Removal of components from the sets and maps starts here.  Be careful
    // using set indices, component references, and map iterators since
    // deletion has side effects.  Deletion of a component might cause another
    // component to be moved within the corresponding set or map.
    bool v0Destroy = (vertex0.GetNumEdges() == 0);
    bool v1Destroy = (vertex1.GetNumEdges() == 0);
    bool v2Destroy = (vertex2.GetNumEdges() == 0);

    // Remove edges if no longer used.
    if (e0Destroy)
    {
        RemoveEdge(label0, label1);
    }

    if (e1Destroy)
    {
        RemoveEdge(label1, label2);
    }

    if (e2Destroy)
    {
        RemoveEdge(label2, label0);
    }

    // Remove vertices if no longer used.
    if (v0Destroy)
    {
        RemoveVertex(label0);
    }

    if (v1Destroy)
    {
        RemoveVertex(label1);
    }

    if (v2Destroy)
    {
        RemoveVertex(label2);
    }

    // Remove triangle (definitely no longer used).
    RemoveTriangle(label0, label1, label2);
    return true;
}
//----------------------------------------------------------------------------
void MTMesh::RemoveVertex (int label)
{
    // Get array location of vertex.
    VIter iter = mVMap.find(MTIVertex(label));
    if (iter == mVMap.end())
    {
        assertion(false, "Vertex does not exist.\n");
        return;
    }
    int v = iter->second;

    // Remove the vertex from the array and from the map.
    int vOld, vNew;
    mVertices.RemoveAt(v, &vOld, &vNew);
    mVMap.erase(iter);

    if (vNew >= 0)
    {
        // The vertex at the end of the array moved into the slot vacated by
        // the deleted vertex.  Update all the components sharing the moved
        // vertex.
        MTVertex& vertex = mVertices[vNew];

        // Inform edges about location change.
        for (int e = 0; e < vertex.GetNumEdges(); ++e)
        {
            MTEdge& edge = mEdges[vertex.GetEdge(e)];
            edge.ReplaceVertex(vOld, vNew);
        }

        // Inform triangles about location change.
        for (int t = 0; t < vertex.GetNumTriangles(); ++t)
        {
            MTTriangle& triangle = mTriangles[vertex.GetTriangle(t)];
            triangle.ReplaceVertex(vOld, vNew);
        }

        iter = mVMap.find(MTIVertex(vertex.GetLabel()));
        assertion(iter != mVMap.end(), "Vertex does not exist.\n");
        iter->second = vNew;
    }
}
//----------------------------------------------------------------------------
void MTMesh::RemoveEdge (int label0, int label1)
{
    // Get array location of edge.
    EIter iter = mEMap.find(MTIEdge(label0, label1));
    if (iter == mEMap.end())
    {
        assertion(false, "Edge does not exist.\n");
        return;
    }
    int e = iter->second;

    // Remove the edge from the array and from the map.
    int eOld, eNew;
    mEdges.RemoveAt(e, &eOld, &eNew);
    mEMap.erase(iter);

    if (eNew >= 0)
    {
        // The edge at the end of the array moved into the slot vacated by
        // the deleted edge.  Update all the components sharing the moved
        // edge.
        MTEdge& edge = mEdges[eNew];

        // Inform vertices about location change.
        MTVertex& vertex0 = mVertices[edge.Vertex(0)];
        MTVertex& vertex1 = mVertices[edge.Vertex(1)];
        vertex0.ReplaceEdge(eOld, eNew);
        vertex1.ReplaceEdge(eOld, eNew);

        // Inform triangles about location change.
        for (int t = 0; t < 2; ++t)
        {
            int tIndex = edge.GetTriangle(t);
            if (tIndex != -1)
            {
                MTTriangle& triangle = mTriangles[tIndex];
                triangle.ReplaceEdge(eOld, eNew);
            }
        }

        iter = mEMap.find(MTIEdge(vertex0.GetLabel(), vertex1.GetLabel()));
        assertion(iter != mEMap.end(), "Edge does not exist.\n");
        iter->second = eNew;
    }
}
//----------------------------------------------------------------------------
void MTMesh::RemoveTriangle (int label0, int label1, int label2)
{
    // Get array location of triangle.
    TIter iter = mTMap.find(MTITriangle(label0, label1, label2));
    if (iter == mTMap.end())
    {
        assertion(false, "Triangle does not exist.\n");
        return;
    }
    int t = iter->second;

    // Remove the triangle from the array and from the map.
    int tOld, tNew;
    mTriangles.RemoveAt(t, &tOld, &tNew);
    mTMap.erase(iter);

    if (tNew >= 0)
    {
        // The triangle at the end of the array moved into the slot vacated by
        // the deleted triangle.  Update all the components sharing the moved
        // triangle.
        MTTriangle& triangle = mTriangles[tNew];

        // Inform vertices about location change.
        MTVertex& vertex0 = mVertices[triangle.Vertex(0)];
        MTVertex& vertex1 = mVertices[triangle.Vertex(1)];
        MTVertex& vertex2 = mVertices[triangle.Vertex(2)];
        vertex0.ReplaceTriangle(tOld, tNew);
        vertex1.ReplaceTriangle(tOld, tNew);
        vertex2.ReplaceTriangle(tOld, tNew);

        // Inform edges about location change.
        for (int e = 0; e < 3; ++e)
        {
            MTEdge& edge = mEdges[triangle.GetEdge(e)];
            edge.ReplaceTriangle(tOld, tNew);
        }

        // Inform adjacents about location change.
        for (int a = 0; a < 3; ++a)
        {
            int aIndex = triangle.GetAdjacent(a);
            if (aIndex != -1)
            {
                MTTriangle& adjacent = mTriangles[aIndex];
                adjacent.ReplaceAdjacent(tOld, tNew);
            }
        }

        iter = mTMap.find(MTITriangle(vertex0.GetLabel(), vertex1.GetLabel(),
            vertex2.GetLabel()));
        assertion(iter != mTMap.end(), "Triangle does not exist.\n");
        iter->second = tNew;
    }
}
//----------------------------------------------------------------------------
void MTMesh::AttachTriangleToEdge (int t, MTTriangle& triangle, int i, int e,
    MTEdge& edge)
{
    if (edge.Triangle(0) == -1)
    {
        edge.Triangle(0) = t;
    }
    else
    {
        int a = edge.Triangle(0);
        MTTriangle& adjacent = mTriangles[a];
        triangle.Adjacent(i) = a;
        for (int j = 0; j < 3; ++j)
        {
            if (adjacent.Edge(j) == e)
            {
                adjacent.Adjacent(j) = t;
                break;
            }
        }

        if (edge.Triangle(1) == -1)
        {
            edge.Triangle(1) = t;
        }
        else
        {
            assertion(false, "The mesh is not manifold.\n");
        }
    }

    triangle.Edge(i) = e;
}
//----------------------------------------------------------------------------
void MTMesh::DetachTriangleFromEdge (int t, MTTriangle& triangle, int i,
    int e, MTEdge& edge)
{
    // This function leaves T only partially complete.  The edge E is no
    // longer referenced by T, even though the vertices of T reference the
    // end points of E.  If T has an adjacent triangle A that shares E, then
    // A is a complete triangle.

    if (edge.Triangle(0) == t)
    {
        int a = edge.Triangle(1);
        if (a != -1)
        {
            // T and TAdj share E, update adjacency information for both
            MTTriangle& adjacent = mTriangles[a];
            for (int j = 0; j < 3; ++j)
            {
                if (adjacent.Edge(j) == e)
                {
                    adjacent.Adjacent(j) = -1;
                    break;
                }
            }
        }
        edge.Triangle(0) = a;
    }
    else if (edge.Triangle(1) == t)
    {
        // T and TAdj share E, update adjacency information for both
        MTTriangle& adjacent = mTriangles[edge.Triangle(0)];
        for (int j = 0; j < 3; ++j)
        {
            if (adjacent.Edge(j) == e)
            {
                adjacent.Adjacent(j) = -1;
                break;
            }
        }
    }
    else
    {
        // Should not get here.  The specified edge must share the input
        // triangle.
        assertion(false, "Unexpected condition.\n");
    }

    edge.Triangle(1) = -1;
    triangle.Edge(i) = -1;
    triangle.Adjacent(i) = -1;
}
//----------------------------------------------------------------------------
bool MTMesh::SubdivideCentroid (int label0, int label1, int label2,
    int& nextLabel)
{
    int t = T(label0,label1,label2);
    if (t == -1)
    {
        return false;
    }

    if (mVMap.find(MTIVertex(nextLabel)) != mVMap.end())
    {
        // Vertex already exists with this label.
        return false;
    }

    // Subdivide the triangle.
    Remove(label0, label1, label2);
    Insert(label0, label1, nextLabel);
    Insert(label1, label2, nextLabel);
    Insert(label2, label0, nextLabel);

    ++nextLabel;
    return true;
}
//----------------------------------------------------------------------------
bool MTMesh::SubdivideCentroidAll (int& nextLabel)
{
    // Verify that the next-label range is valid.
    const int tMax = mTriangles.GetNumElements();
    int t, tempLabel;
    for (t = 0, tempLabel = nextLabel; t < tMax; ++t, ++tempLabel)
    {
        if (mVMap.find(MTIVertex(tempLabel)) != mVMap.end())
        {
            // A vertex already exists with this label.
            return false;
        }
    }

    // Care must be taken when processing the triangles iteratively.  The
    // side of effect of removing the first triangle is that the last triangle
    // in the array is moved into the vacated position.  The first problem is
    // that the moved triangle will be skipped in the iteration.  The second
    // problem is that the insertions cause the triangle array to grow.  To
    // avoid skipping the moved triangle, a different algorithm than the one
    // in SubdivideCentroid(int,int,int,int&) is used.  The triangle to be
    // removed is detached from two edges.  Two of the subtriangles are added
    // to the mesh.  The third subtriangle is calculated in the already
    // existing memory that stored the original triangle.  To avoid the
    // infinite recursion induced by a growing array, the original size of
    // the triangle array is stored int iTMax.  This guarantees that only the
    // original triangles are subdivided and that newly added triangles are
    // not.
    for (t = 0; t < tMax; ++t, ++nextLabel)
    {
        // The triangle to subdivide.
        MTTriangle& triangle = mTriangles[t];
        int label0 = GetVLabel(triangle.Vertex(0));
        int label1 = GetVLabel(triangle.Vertex(1));
        int label2 = GetVLabel(triangle.Vertex(2));

        // Detach the triangle from two edges.
        int e1 = triangle.Edge(1), e2 = triangle.Edge(2);
        MTEdge& edge1 = mEdges[e1];
        MTEdge& edge2 = mEdges[e2];
        DetachTriangleFromEdge(t, triangle, 1, e1, edge1);
        DetachTriangleFromEdge(t, triangle, 2, e2, edge2);

        // Insert the two subtriangles that share edges E1 and E2.  A
        // potential side effect is that the triangle array is reallocated
        // to make room for the new triangles.  This will invalidate the
        // reference 'triangle' from the code above, but the index t into
        // the array is still correct.  A reallocation of the vertex array
        // might also occur.
        Insert(label1, label2, nextLabel);
        Insert(label2, label0, nextLabel);

        // Stitch the third subtriangle to the other subtriangles.
        MTTriangle& triangleN = mTriangles[t];
        int subE1 = E(label1, nextLabel);
        int subE2 = E(label0, nextLabel);
        MTEdge& subEdge1 = mEdges[subE1];
        MTEdge& subEdge2 = mEdges[subE2];
        AttachTriangleToEdge(t, triangleN, 1, subE1, subEdge1);
        AttachTriangleToEdge(t, triangleN, 2, subE2, subEdge2);
    }

    return true;
}
//----------------------------------------------------------------------------
bool MTMesh::SubdivideEdge (int label0, int label1, int& nextLabel)
{
    int e = E(label0, label1);
    if (e == -1)
    {
        return false;
    }

    if (mVMap.find(MTIVertex(nextLabel)) != mVMap.end())
    {
        // A vertex already exists with this label.
        return false;
    }

    // Split the triangles sharing the edge.
    MTEdge& edge = mEdges[e];
    int t0 = edge.Triangle(0);
    int t1 = edge.Triangle(1);
    int t0v0, t0v1, t0v2, t1v0, t1v1, t1v2, t0e0, t0e1, t1e0, t1e1;
    if (t0 >= 0 && t1 == -1)
    {
        // The edge is shared only by T0.
        MTTriangle& triangle0 = mTriangles[t0];
        t0v0 = GetVLabel(triangle0.Vertex(0));
        t0v1 = GetVLabel(triangle0.Vertex(1));
        t0v2 = GetVLabel(triangle0.Vertex(2));
        t0e0 = triangle0.Edge(0);
        t0e1 = triangle0.Edge(1);

        Remove(t0v0, t0v1, t0v2);
        if (t0e0 == e)
        {
            Insert(t0v0, nextLabel, t0v2);
            Insert(nextLabel, t0v1, t0v2);
        }
        else if (t0e1 == e)
        {
            Insert(t0v1, nextLabel, t0v0);
            Insert(nextLabel, t0v2, t0v0);
        }
        else
        {
            Insert(t0v2, nextLabel, t0v1);
            Insert(nextLabel, t0v0, t0v1);
        }
    }
    else if (t1 >= 0 && t0 == -1)
    {
        // The edge is shared only by T1.  The Remove(int,int,int) call is not
        // factored outside the conditional statements to avoid potential
        // reallocation side effects that would invalidate the reference
        // 'triangle1'.
        MTTriangle& triangle1 = mTriangles[t1];
        t1v0 = GetVLabel(triangle1.Vertex(0));
        t1v1 = GetVLabel(triangle1.Vertex(1));
        t1v2 = GetVLabel(triangle1.Vertex(2));
        t1e0 = triangle1.Edge(0);
        t1e1 = triangle1.Edge(1);

        Remove(t1v0, t1v1, t1v2);
        if (t1e0 == e)
        {
            Insert(t1v0, nextLabel, t1v2);
            Insert(nextLabel, t1v1, t1v2);
        }
        else if (t1e1 == e)
        {
            Insert(t1v1, nextLabel, t1v0);
            Insert(nextLabel, t1v2, t1v0);
        }
        else
        {
            Insert(t1v2, nextLabel, t1v1);
            Insert(nextLabel, t1v0, t1v1);
        }
    }
    else
    {
        assertion(t0 >= 0 && t1 >= 0, "Unexpected condition.\n");

        // The edge is shared both by T0 and T1.  The Remove(int,int,int) call
        // is not factored outside the conditional statements to avoid
        // potential reallocation side effects that would invalidate the
        /// references 'triangle0' and 'triangle1'.
        MTTriangle& triangle0 = mTriangles[t0];
        t0v0 = GetVLabel(triangle0.Vertex(0));
        t0v1 = GetVLabel(triangle0.Vertex(1));
        t0v2 = GetVLabel(triangle0.Vertex(2));
        t0e0 = triangle0.Edge(0);
        t0e1 = triangle0.Edge(1);

        MTTriangle& triangle1 = mTriangles[t1];
        t1v0 = GetVLabel(triangle1.Vertex(0));
        t1v1 = GetVLabel(triangle1.Vertex(1));
        t1v2 = GetVLabel(triangle1.Vertex(2));
        t1e0 = triangle1.Edge(0);
        t1e1 = triangle1.Edge(1);

        // Both triangles must be removed before the insertions to guarantee
        // that the common edge is deleted first from the mesh.
        Remove(t0v0, t0v1, t0v2);
        Remove(t1v0, t1v1, t1v2);

        if (t0e0 == e)
        {
            Insert(t0v0, nextLabel, t0v2);
            Insert(nextLabel, t0v1, t0v2);
        }
        else if (t0e1 == e)
        {
            Insert(t0v1, nextLabel, t0v0);
            Insert(nextLabel, t0v2, t0v0);
        }
        else
        {
            Insert(t0v2, nextLabel, t0v1);
            Insert(nextLabel, t0v0, t0v1);
        }

        if (t1e0 == e)
        {
            Insert(t1v0, nextLabel, t1v2);
            Insert(nextLabel, t1v1, t1v2);
        }
        else if (t1e1 == e)
        {
            Insert(t1v1, nextLabel, t1v0);
            Insert(nextLabel, t1v2, t1v0);
        }
        else
        {
            Insert(t1v2, nextLabel, t1v1);
            Insert(nextLabel, t1v0, t1v1);
        }
    }

    ++nextLabel;
    return true;
}
//----------------------------------------------------------------------------
void MTMesh::Print (std::ofstream& outFile) const
{
    int v, e, t;

    // Print the vertex information.
    const int numVertices = mVertices.GetNumElements();
    outFile << "vertex quantity = " << numVertices << std::endl;
    for (v = 0; v < numVertices; ++v)
    {
        const MTVertex& vertex = mVertices.Get(v);

        outFile << "vertex<" << v << ">" << std::endl;
        outFile << "    l: " << vertex.GetLabel() << std::endl;
        outFile << "    e: ";
        const int numEdges = vertex.GetNumEdges();
        for (e = 0; e < numEdges; ++e)
        {
            outFile << vertex.GetEdge(e) << ' ';
        }
        outFile << std::endl;
        outFile << "    t: ";
        const int numTriangles = vertex.GetNumTriangles();
        for (t = 0; t < numTriangles; ++t)
        {
            outFile << vertex.GetTriangle(t) << ' ';
        }
        outFile << std::endl;
    }
    outFile << std::endl;

    // Print the edge information.
    const int numEdges = mEdges.GetNumElements();
    outFile << "edge quantity = " << numEdges << std::endl;
    for (e = 0; e < numEdges; ++e)
    {
        const MTEdge& edge = mEdges.Get(e);

        outFile << "edge<" << e << ">" << std::endl;
        outFile << "    v: " << edge.GetVertex(0) << ' ' << edge.GetVertex(1)
            << std::endl;
        outFile << "    t: " << edge.GetTriangle(0) << ' '
            << edge.GetTriangle(1) << std::endl;
    }
    outFile << std::endl;

    // Print the triangle information.
    const int numTriangles = mTriangles.GetNumElements();
    outFile << "triangle quantity = " << numTriangles << std::endl;
    for (t = 0; t < numTriangles; ++t)
    {
        const MTTriangle& triangle = mTriangles.Get(t);

        outFile << "triangle<" << t << ">" << std::endl;
        outFile << "    v: "
            << triangle.GetVertex(0) << ' '
            << triangle.GetVertex(1) << ' '
            << triangle.GetVertex(2) << std::endl;
        outFile << "    e: "
            << triangle.GetEdge(0) << ' '
            << triangle.GetEdge(1) << ' '
            << triangle.GetEdge(2) << std::endl;
        outFile << "    a: "
            << triangle.GetAdjacent(0) << ' '
            << triangle.GetAdjacent(1) << ' '
            << triangle.GetAdjacent(2) << std::endl;
    }
    outFile << std::endl;
}
//----------------------------------------------------------------------------
bool MTMesh::Print (const char* filename) const
{
    std::ofstream outFile(filename);
    if (!outFile)
    {
        return false;
    }

    Print(outFile);
    return true;
}
//----------------------------------------------------------------------------
