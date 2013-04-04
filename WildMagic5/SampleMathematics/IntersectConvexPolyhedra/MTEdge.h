// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef MTEDGE_H
#define MTEDGE_H

class MTEdge
{
public:
    MTEdge (int label = -1);
    MTEdge (const MTEdge& edge);
    virtual ~MTEdge ();

    MTEdge& operator= (const MTEdge& edge);

    inline int GetLabel () const;
    inline int& Label ();

    inline int GetVertex (int i) const;
    inline int& Vertex (int i);
    bool ReplaceVertex (int vOld, int vNew);

    inline int GetTriangle (int i) const;
    inline int& Triangle (int i);
    bool ReplaceTriangle (int tOld, int tNew);

    bool operator== (const MTEdge& edge) const;

protected:
    int mLabel;
    int mVertex[2];
    int mTriangle[2];
};

#include "MTEdge.inl"

#endif
