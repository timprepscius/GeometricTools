// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef SEGMENTGRAPH
#define SEGMENTGRAPH

#include "Rational.h"

class SegmentGraph
{
public:
    SegmentGraph ();
    ~SegmentGraph ();

    class Vertex
    {
    public:
        Vertex ();
        ~Vertex ();

        void InsertAdjacent (Vertex* adjacent);
        void RemoveAdjacent (Vertex* adjacent);

        enum { VG_CHUNK = 16 };
        RPoint2 Position;
        int NumVertices;
        Vertex** V;
    };

    class Edge
    {
    public:
        Edge ();
        Edge (Vertex* v0, Vertex* v1);

        void SetVertices (Vertex* v0, Vertex* v1);
        Vertex* GetVertex (int i) const;
        bool operator== (const Edge& edge) const;
        bool operator< (const Edge& edge) const;

    private:
        Vertex* mV[2];
    };

    typedef std::map<RPoint2,Vertex*> VMap;
    typedef std::set<Edge> ESet;

    VMap& GetVertices ();
    ESet& GetEdges ();

    void InsertEdge (const RPoint2& point0, const RPoint2& point1);
    bool RemoveEdge (const RPoint2& point0, const RPoint2& point1);
    void ExtractEnvelope (std::vector<RPoint2>& envelope);

private:
    Vertex* GetVertex (const RPoint2& point);
    Vertex* InsertVertex (const RPoint2& point);

    VMap mVertexMap;
    ESet mEdgeSet;
};

#endif
