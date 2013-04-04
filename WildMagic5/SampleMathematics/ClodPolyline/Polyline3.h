// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef POLYLINE3_H
#define POLYLINE3_H

#include "Wm5Vector3.h"
using namespace Wm5;

class Polyline3
{
public:
    // Constructor and destructor.  The quantity Q must be 2 or larger.
    // Polyline3 accepts responsibility for deleting the input array.  The
    // vertices are assumed to be ordered.  For an open polyline, the segments
    // are <V[i],V[i+1]> for 0 <= i <= Q-2.  If the polyline is closed, an
    // additional segment is <V[Q-1],V[0]>.
    Polyline3 (int numVertices, Vector3f* vertices, bool closed);
    ~Polyline3 ();

    // Accessors to vertex data.
    inline int GetNumVertices () const;
    inline const Vector3f* GetVertices () const;
    inline bool GetClosed () const;

    // Accessors to edge data.
    inline int GetNumEdges () const;
    inline const int* GetEdges () const;

    // Accessors to level of detail (MinLOD <= LOD <= MaxLOD is required).
    inline int GetMinLevelOfDetail () const;
    inline int GetMaxLevelOfDetail () const;
    inline int GetLevelOfDetail () const;
    void SetLevelOfDetail (int lod);

protected:
    // The polyline vertices.
    int mNumVertices;
    Vector3f* mVertices;
    bool mClosed;

    // The polyline edges.
    int mNumEdges;
    int* mEdges;

    // The level of detail information.
    int mVMin, mVMax;
    int* mIndexMap;
};

#include "Polyline3.inl"

#endif
