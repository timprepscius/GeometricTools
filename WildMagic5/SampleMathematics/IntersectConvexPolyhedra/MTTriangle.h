// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef MTTRIANGLE_H
#define MTTRIANGLE_H

class MTTriangle
{
public:
    MTTriangle (int label = -1);
    MTTriangle (const MTTriangle& triangle);
    virtual ~MTTriangle ();

    MTTriangle& operator= (const MTTriangle& triangle);

    inline int GetLabel () const;
    inline int& Label ();

    inline int GetVertex (int i) const;
    inline int& Vertex (int i);
    bool ReplaceVertex (int vOld, int vNew);

    inline int GetEdge (int i) const;
    inline int& Edge (int i);
    bool ReplaceEdge (int eOld, int eNew);

    inline int GetAdjacent (int i) const;
    inline int& Adjacent (int i);
    bool ReplaceAdjacent (int aOld, int aNew);

    bool operator== (const MTTriangle& triangle) const;

protected:
    int mLabel;
    int mVertex[3];
    int mEdge[3];
    int mAdjacent[3];
};

#include "MTTriangle.inl"

#endif
