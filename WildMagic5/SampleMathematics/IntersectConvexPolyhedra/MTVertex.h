// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef MTVERTEX_H
#define MTVERTEX_H

#include "UnorderedSet.h"

class MTVertex
{
public:
    // Construction and destruction.
    MTVertex (int label = -1, int eGrow = 0, int tGrow = 0);
    MTVertex (const MTVertex& V);
    virtual ~MTVertex ();

    MTVertex& operator= (const MTVertex& vertex);

    // Vertex labels are read-only since they are used for maps in the MTMesh
    // class for inverse look-up.
    inline int GetLabel () const;

    inline int GetNumEdges () const;
    inline int GetEdge (int e) const;
    inline bool InsertEdge (int e);
    inline bool RemoveEdge (int e);
    bool ReplaceEdge (int eOld, int eNew);

    inline int GetNumTriangles () const;
    inline int GetTriangle (int t) const;
    inline bool InsertTriangle (int t);
    inline bool RemoveTriangle (int t);
    bool ReplaceTriangle(int tOld, int tNew);

    inline bool operator== (const MTVertex& vertex) const;

protected:
    int mLabel;
    UnorderedSet<int> mESet, mTSet;
};

#include "MTVertex.inl"

#endif
