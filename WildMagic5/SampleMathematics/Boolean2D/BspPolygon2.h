// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef BSPPOLYGON2_H
#define BSPPOLYGON2_H

#include "Types2.h"

class BspTree2;

class BspPolygon2
{
public:
    // Construction and destruction.
    BspPolygon2 ();
    BspPolygon2 (const BspPolygon2& polygon);
    ~BspPolygon2 ();

    // Support for deferred construction.
    int InsertVertex (const Vector2d& vertex);
    int InsertEdge (const Edge2& edge);
    void Finalize ();

    // Assignment.
    BspPolygon2& operator= (const BspPolygon2& polygon);

    // Member access.
    int GetNumVertices () const;
    bool GetVertex (int i, Vector2d& vertex) const;
    int GetNumEdges () const;
    bool GetEdge (int i, Edge2& edge) const;

    // negation
    BspPolygon2 operator~ () const;

    // intersection
    BspPolygon2 operator& (const BspPolygon2& polygon) const;

    // union
    BspPolygon2 operator| (const BspPolygon2& polygon) const;

    // difference
    BspPolygon2 operator- (const BspPolygon2& polygon) const;

    // exclusive or
    BspPolygon2 operator^ (const BspPolygon2& polygon) const;

    // point location (-1 inside, 0 on polygon, 1 outside)
    int PointLocation (const Vector2d& vertex) const;

    void Print (const char* filename) const;

protected:
    void SplitEdge (int v0, int v1, int vmid);
    void GetInsideEdgesFrom (const BspPolygon2& polygon, BspPolygon2& inside)
        const;

    // vertices
    VMap mVMap;
    VArray mVArray;

    // edges
    EMap mEMap;
    EArray mEArray;

    friend class BspTree2;
    BspTree2* mTree;
};

#endif
