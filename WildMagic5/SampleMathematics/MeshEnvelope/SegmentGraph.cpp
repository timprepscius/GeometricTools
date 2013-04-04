// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "SegmentGraph.h"
#include "Wm5Memory.h"

//----------------------------------------------------------------------------
SegmentGraph::SegmentGraph ()
{
}
//----------------------------------------------------------------------------
SegmentGraph::~SegmentGraph ()
{
    VMap::iterator iter = mVertexMap.begin();
    VMap::iterator end = mVertexMap.end();
    for (/**/; iter != end; ++iter)
    {
        Vertex* vertex = iter->second;
        delete0(vertex);
    }
}
//----------------------------------------------------------------------------
SegmentGraph::VMap& SegmentGraph::GetVertices ()
{
    return mVertexMap;
}
//----------------------------------------------------------------------------
SegmentGraph::ESet& SegmentGraph::GetEdges ()
{
    return mEdgeSet;
}
//----------------------------------------------------------------------------
void SegmentGraph::InsertEdge (const RPoint2& point0, const RPoint2& point1)
{
    // Insert vertices into the vertex set.  If the points already exist,
    // the function call is just a lookup; otherwise, new vertices are
    // allocated.
    Vertex* vertex0 = InsertVertex(point0);
    Vertex* vertex1 = InsertVertex(point1);

    Edge edge(vertex0, vertex1);
    std::set<Edge>::iterator iter = mEdgeSet.find(edge);
    if (iter == mEdgeSet.end())
    {
        // The edge does not exist, insert it into the set.
        mEdgeSet.insert(edge);

        // Update the vertex-adjacency information.
        vertex0->InsertAdjacent(vertex1);
        vertex1->InsertAdjacent(vertex0);
    }
}
//----------------------------------------------------------------------------
bool SegmentGraph::RemoveEdge (const RPoint2& point0, const RPoint2& point1)
{
    // Look up the vertices.  If one or the other does not exist, there is
    // nothing to do.
    Vertex* vertex0 = GetVertex(point0);
    if (!vertex0)
    {
        return false;
    }

    Vertex* vertex1 = GetVertex(point1);
    if (!vertex1)
    {
        return false;
    }

    Edge edge(vertex0, vertex1);
    std::set<Edge>::iterator iter = mEdgeSet.find(edge);
    if (iter != mEdgeSet.end())
    {
        // The edge exists, remove it from the set.
        mEdgeSet.erase(edge);

        // Update the vertex-adjacency information.
        vertex0->RemoveAdjacent(vertex1);
        vertex1->RemoveAdjacent(vertex0);

        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void SegmentGraph::ExtractEnvelope (std::vector<RPoint2>& envelope)
{
    // Get the left-most (min x) and bottom-most (min y) vertex of the graph.
    VMap::iterator iter = mVertexMap.begin();
    VMap::iterator end = mVertexMap.end();
    Vertex* vMin = iter->second;
    for (++iter; iter != end; ++iter)
    {
        Vertex* vertex = iter->second;
        if (vertex->Position.X() < vMin->Position.X())
        {
            vMin = vertex;
        }
        else if (vertex->Position.X() == vMin->Position.X())
        {
            if (vertex->Position.Y() < vMin->Position.Y())
            {
                vMin = vertex;
            }
        }
    }

    // Traverse the envelope.  If Vcurr is the current vertex and Vprev is
    // its predecessor in the envelope, then the successor Vnext is selected
    // to be a vertex adjacent to Vcurr according to the following.  The
    // current edge has direction Dcurr = Vcurr - Vprev.  Vnext is an adjacent
    // vertex of Vcurr, selected so that the direction Dnext = Vnext - Vcurr
    // has the property that Dnext forms the largest interior angle at Vcurr
    // compared to -Dcurr, the angle measured by rotating -Dcurr clockwise
    // about Vcurr.  The starting vertex Vmin does not have a predecessor, but
    // the construction of Vmin implies a starting direction Dcurr = (0,-1).
    Vertex* vCurr = vMin;
    Vertex* vPrev = 0;
    Vertex* vNext = 0;
    RPoint2 dirCurr(0,-1), dirNext;
    bool vCurrConvex = true;

    // The loop could be a 'while' loop, terminating when the Vnext becomes
    // Vmin, but to trap potential errors that cause an infinite loop, a for
    // loop is used to guarantee bounded iterations.  The upper bound of the
    // loop is the worst-case behavior (each vertex connected to all other
    // vertices).
    const int numVertices = (int)mVertexMap.size();
    const int maxNumVertices = numVertices*(numVertices - 1)/2;
    int i;
    for (i = 0; i < maxNumVertices; ++i)
    {
        envelope.push_back(vCurr->Position);

        // Search the adjacent vertices for Vnext.
        for (int j = 0; j < vCurr->NumVertices; ++j)
        {
            Vertex* vertex = vCurr->V[j];
            if (vertex == vPrev)
            {
                continue;
            }

            RPoint2 dir = vertex->Position - vCurr->Position;
            if (!vNext)
            {
                vNext = vertex;
                dirNext = dir;
                vCurrConvex = (dirNext.DotPerp(dirCurr) <= 0);
                continue;
            }

            if (vCurrConvex)
            {
                if (dirCurr.DotPerp(dir) < 0 || dirNext.DotPerp(dir) < 0)
                {
                    vNext = vertex;
                    dirNext = dir;
                    vCurrConvex = (dirNext.DotPerp(dirCurr) <= 0);
                }
            }
            else
            {
                if (dirCurr.DotPerp(dir) < 0 && dirNext.DotPerp(dir) < 0)
                {
                    vNext = vertex;
                    dirNext = dir;
                    vCurrConvex = (dirNext.DotPerp(dirCurr) <= 0);
                }
            }
        }

        assertion(vNext != 0, "Unexpected condition.\n");
        vPrev = vCurr;
        vCurr = vNext;
        vNext = 0;
        dirCurr = dirNext;

        if (vCurr == vMin)
        {
            break;
        }
    }
    assertion(i < numVertices, "Unexpected condition.\n");
}
//----------------------------------------------------------------------------
SegmentGraph::Vertex* SegmentGraph::GetVertex (const RPoint2& point)
{
    VMap::iterator iter = mVertexMap.find(point);
    return (iter != mVertexMap.end() ? iter->second : 0);
}
//----------------------------------------------------------------------------
SegmentGraph::Vertex* SegmentGraph::InsertVertex (const RPoint2& point)
{
    VMap::iterator iter = mVertexMap.find(point);
    if (iter != mVertexMap.end())
    {
        return iter->second;
    }

    // Insert the vertex into the vertex set.  The adjacency array has already
    // been initialized to empty.
    Vertex* vertex = new0 Vertex();
    vertex->Position = point;
    mVertexMap[point] = vertex;
    return vertex;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// SegmentGraph::Vertex
//----------------------------------------------------------------------------
SegmentGraph::Vertex::Vertex ()
    :
    NumVertices(0),
    V(0)
{
    // Position is uninitialized.
}
//----------------------------------------------------------------------------
SegmentGraph::Vertex::~Vertex ()
{
    delete1(V);
}
//----------------------------------------------------------------------------
void SegmentGraph::Vertex::InsertAdjacent (Vertex* adjacent)
{
    // Grow the vertex array if necessary.
    if ((NumVertices % VG_CHUNK) == 0)
    {
        size_t numBytes = NumVertices*sizeof(Vertex*);
        Vertex** save = V;
        V = new1<Vertex*>(NumVertices + VG_CHUNK);
        if (save)
        {
            memcpy(V, save, numBytes);
            delete1(save);
        }
    }

    V[NumVertices++] = adjacent;
}
//----------------------------------------------------------------------------
void SegmentGraph::Vertex::RemoveAdjacent (Vertex* adjacent)
{
    for (int i = 0; i < NumVertices; ++i)
    {
        if (adjacent == V[i])
        {
            // Maintain a compact array.
            if (i < --NumVertices)
            {
                V[i] = V[NumVertices];
            }
            V[NumVertices] = 0;
            return;
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// SegmentGraph::Edge
//----------------------------------------------------------------------------
SegmentGraph::Edge::Edge ()
{
    mV[0] = 0;
    mV[1] = 0;
}
//----------------------------------------------------------------------------
SegmentGraph::Edge::Edge (Vertex* vertex0, Vertex* vertex1)
{
    SetVertices(vertex0, vertex1);
}
//----------------------------------------------------------------------------
void SegmentGraph::Edge::SetVertices (Vertex* vertex0, Vertex* vertex1)
{
    if (vertex0 < vertex1)
    {
        // V0 is minimum.
        mV[0] = vertex0;
        mV[1] = vertex1;
    }
    else
    {
        // V1 is minimum.
        mV[0] = vertex1;
        mV[1] = vertex0;
    }
}
//----------------------------------------------------------------------------
SegmentGraph::Vertex* SegmentGraph::Edge::GetVertex (int i) const
{
    return mV[i];
}
//----------------------------------------------------------------------------
bool SegmentGraph::Edge::operator== (const Edge& edge) const
{
    return mV[0] == edge.mV[0] && mV[1] == edge.mV[1];
}
//----------------------------------------------------------------------------
bool SegmentGraph::Edge::operator< (const Edge& edge) const
{
    if (mV[1] < edge.mV[1])
    {
        return true;
    }

    if (mV[1] > edge.mV[1])
    {
        return false;
    }

    return mV[0] < edge.mV[0];
}
//----------------------------------------------------------------------------
