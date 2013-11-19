// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "BspPolygon2.h"
#include "BspTree2.h"
#include "Wm5Memory.h"
using namespace Wm5;

//----------------------------------------------------------------------------
BspPolygon2::BspPolygon2 ()
{
    mTree = 0;
}
//----------------------------------------------------------------------------
BspPolygon2::BspPolygon2 (const BspPolygon2& polygon)
{
    mTree = 0;
    *this = polygon;
}
//----------------------------------------------------------------------------
BspPolygon2::~BspPolygon2 ()
{
    delete0(mTree);
}
//----------------------------------------------------------------------------
BspPolygon2& BspPolygon2::operator= (const BspPolygon2& polygon)
{
    mVMap = polygon.mVMap;
    mVArray = polygon.mVArray;
    mEMap = polygon.mEMap;
    mEArray = polygon.mEArray;
    delete0(mTree);
    mTree = (polygon.mTree ? polygon.mTree->GetCopy() : 0);
    return *this;
}
//----------------------------------------------------------------------------
int BspPolygon2::InsertVertex (const Vector2d& vertex)
{
    VIterator iter = mVMap.find(vertex);
    if (iter != mVMap.end())
    {
        // Vertex already in map, just return its unique index.
        return iter->second;
    }

    // Vertex not in map, insert it and assign it a unique index.
    int i = (int)mVArray.size();
    mVMap.insert(std::make_pair(vertex, i));
    mVArray.push_back(vertex);
    return i;
}
//----------------------------------------------------------------------------
int BspPolygon2::InsertEdge (const Edge2& edge)
{
    assertion(edge.I0 != edge.I1, "Degenerate edges not allowed.\n");

    EIterator iter = mEMap.find(edge);
    if (iter != mEMap.end())
    {
        // Edge already in map, just return its unique index.
        return iter->second;
    }

    // Edge not in map, insert it and assign it a unique index.
    int i = (int)mEArray.size();
    mEMap.insert(std::make_pair(edge, i));
    mEArray.push_back(edge);
    return i;
}
//----------------------------------------------------------------------------
void BspPolygon2::SplitEdge (int v0, int v1, int vmid)
{
    // Find the edge in the map to get the edge-array index.
    EIterator iter = mEMap.find(Edge2(v0, v1));
    assertion(iter != mEMap.end(), "Edge does not exist in the map.\n");
    int eIndex = iter->second;

    // Delete edge <V0,V1>.
    mEMap.erase(iter);

    // Insert edge <V0,VM>.
    mEArray[eIndex].I1 = vmid;
    mEMap.insert(std::make_pair(mEArray[eIndex], eIndex));

    // Insert edge <VM,V1>.
    InsertEdge(Edge2(vmid, v1));
}
//----------------------------------------------------------------------------
void BspPolygon2::Finalize ()
{
    delete0(mTree);
    mTree = new0 BspTree2(*this, mEArray);
}
//----------------------------------------------------------------------------
int BspPolygon2::GetNumVertices () const
{
    return (int)mVMap.size();
}
//----------------------------------------------------------------------------
bool BspPolygon2::GetVertex (int i, Vector2d& vertex) const
{
    if (0 <= i && i < (int)mVArray.size())
    {
        vertex = mVArray[i];
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
int BspPolygon2::GetNumEdges () const
{
    return (int)mEMap.size();
}
//----------------------------------------------------------------------------
bool BspPolygon2::GetEdge (int i, Edge2& edge) const
{
    if (0 <= i && i < (int)mEArray.size())
    {
        edge = mEArray[i];
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void BspPolygon2::GetInsideEdgesFrom (const BspPolygon2& polygon,
    BspPolygon2& inside) const
{
    assertion(mTree != 0, "Tree must exist.\n");

    BspPolygon2 ignore;
    const int numEdges = polygon.GetNumEdges();
    for (int i = 0; i < numEdges; ++i)
    {
        int v0 = polygon.mEArray[i].I0;
        int v1 = polygon.mEArray[i].I1;
        Vector2d vertex0 = polygon.mVArray[v0];
        Vector2d vertex1 = polygon.mVArray[v1];
        mTree->GetPartition(*this, vertex0, vertex1, ignore, inside, inside,
            ignore);
    }
}
//----------------------------------------------------------------------------
BspPolygon2 BspPolygon2::operator~ () const
{
    assertion(mTree != 0, "Tree must exist.\n");

    // negation
    BspPolygon2 neg;
    neg.mVMap = mVMap;
    neg.mVArray = mVArray;
    ECIterator iter = mEMap.begin();
    ECIterator end = mEMap.end();
    for (/**/; iter != end; ++iter)
    {
        neg.InsertEdge(Edge2(iter->first.I1, iter->first.I0));
    }

    neg.mTree = mTree->GetCopy();
    neg.mTree->Negate();
    return neg;
}
//----------------------------------------------------------------------------
BspPolygon2 BspPolygon2::operator& (const BspPolygon2& polygon) const
{
    assertion(mTree != 0, "Tree must exist.\n");

    // intersection
    BspPolygon2 intersect;
    GetInsideEdgesFrom(polygon, intersect);
    polygon.GetInsideEdgesFrom(*this, intersect);
    intersect.Finalize();
    return intersect;
}
//----------------------------------------------------------------------------
BspPolygon2 BspPolygon2::operator| (const BspPolygon2& polygon) const
{
    // union
    const BspPolygon2& thisPolygon = *this;
    return ~(~thisPolygon & ~polygon);
}
//----------------------------------------------------------------------------
BspPolygon2 BspPolygon2::operator- (const BspPolygon2& polygon) const
{
    // difference
    const BspPolygon2& thisPolygon = *this;
    return thisPolygon & ~polygon;
}
//----------------------------------------------------------------------------
BspPolygon2 BspPolygon2::operator^ (const BspPolygon2& polygon) const
{
    // exclusive or
    const BspPolygon2& thisPolygon = *this;
    return (thisPolygon - polygon) | (polygon - thisPolygon);
}
//----------------------------------------------------------------------------
int BspPolygon2::PointLocation (const Vector2d& vertex) const
{
    assertion(mTree != 0, "Tree must exist.\n");
    return mTree->PointLocation(*this, vertex);
}
//----------------------------------------------------------------------------
void BspPolygon2::Print (const char* filename) const
{
    std::ofstream outFile(filename);

    const int numVertices = (int)mVArray.size();
    outFile << "vquantity = " << numVertices << std::endl;
    int i;
    for (i = 0; i < numVertices; ++i)
    {
        outFile << i << "  (" << mVArray[i].X() << ',' << mVArray[i].Y()
            << ')' << std::endl;
    }
    outFile << std::endl;

    const int numEdges = (int)mEArray.size();
    outFile << "equantity = " << numEdges << std::endl;
    for (i = 0; i < numEdges; ++i)
    {
        outFile << "  <" << mEArray[i].I0 << ',' << mEArray[i].I1
            << '>' << std::endl;
    }
    outFile << std::endl;

    outFile << "bsp tree" << std::endl;
    if (mTree)
    {
        mTree->Print(outFile, 0, 'r');
    }
    outFile << std::endl;
}
//----------------------------------------------------------------------------
