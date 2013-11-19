// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef MTMESH_H
#define MTMESH_H

// Manifold Triangle Mesh.  Each edge has 1 or 2 triangles sharing it.

#include "MTIVertex.h"
#include "MTIEdge.h"
#include "MTITriangle.h"
#include "MTVertex.h"
#include "MTEdge.h"
#include "MTTriangle.h"

class MTMesh
{
public:
    MTMesh (int numVertices = 0, int numEdges = 0, int numTriangles = 0);
    MTMesh (const MTMesh& mesh);
    virtual ~MTMesh ();

    void Reset (int numVertices = 0, int numEdges = 0, int numTriangles = 0);
    MTMesh& operator= (const MTMesh& mesh);

    inline int GetNumVertices () const;
    inline int V (int label) const;
    inline const MTVertex& GetVertex (int vIndex) const;
    inline int GetVLabel (int vIndex) const;

    inline int GetNumEdges () const;
    inline int E (int label0, int label1) const;
    inline const MTEdge& GetEdge (int eIndex) const;
    inline int GetELabel (int eIndex) const;
    inline int& ELabel (int eIndex);

    inline int GetNumTriangles () const;
    inline int T (int label0, int label1, int label2) const;
    inline const MTTriangle& GetTriangle (int tIndex) const;
    inline int GetTLabel (int tIndex) const;
    inline int& TLabel (int tIndex);

    inline int& InitialELabel ();
    inline int& InitialTLabel ();

    bool Insert (int label0, int label1, int label2);
    bool Remove (int label0, int label1, int label2);

    bool SubdivideCentroid (int label0, int label1, int label2,
        int& nextLabel);
    bool SubdivideCentroidAll (int& nextLabel);

    bool SubdivideEdge (int label0, int label1, int& nextLabel);

    virtual void Print (std::ofstream& outFile) const;
    virtual bool Print (const char* filename) const;

protected:
    void AttachTriangleToEdge (int t, MTTriangle& triangle, int i, int e,
        MTEdge& edge);
    int InsertVertex (int label);
    int InsertEdge (int label0, int label1);
    int InsertTriangle (int label0, int label1, int label2);

    void DetachTriangleFromEdge (int t, MTTriangle& triangle, int i, int e,
        MTEdge& edge);
    void RemoveVertex (int label);
    void RemoveEdge (int label0, int label1);
    void RemoveTriangle (int label0, int label1, int label2);

    UnorderedSet<MTVertex> mVertices;
    UnorderedSet<MTEdge> mEdges;
    UnorderedSet<MTTriangle> mTriangles;

    int mInitialELabel;
    int mInitialTLabel;

    typedef std::map<MTIVertex,int> VMap;
    typedef std::map<MTIEdge,int> EMap;
    typedef std::map<MTITriangle,int> TMap;
    typedef VMap::iterator VIter;
    typedef EMap::iterator EIter;
    typedef TMap::iterator TIter;
    typedef VMap::const_iterator VCIter;
    typedef EMap::const_iterator ECIter;
    typedef TMap::const_iterator TCIter;

    VMap mVMap;
    EMap mEMap;
    TMap mTMap;
};

#include "MTMesh.inl"

#endif
