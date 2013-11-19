// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef CONVEXCLIPPER_H
#define CONVEXCLIPPER_H

#include "Wm5Plane3.h"
using namespace Wm5;

template <class Real> class ConvexPolyhedron;

template <class Real>
class ConvexClipper
{
public:
    class Vertex
    {
    public:
        Vertex ();

        Vector3<Real> Point;
        Real Distance;
        int Occurs;
        bool Visible;
    };

    class Edge
    {
    public:
        Edge ();

        int Vertex[2];
        int Face[2];
        bool Visible;
    };

    class Face
    {
    public:
        Face ();

        Plane3<Real> Plane;
        std::set<int> Edges;
        bool Visible;
    };

    // Construction.
    ConvexClipper (const ConvexPolyhedron<Real>& polyhedron,
        Real epsilon = (Real)0);

    // Discard the portion of the mesh on the negative side of the plane.
    // This function is valid for any manifold triangle mesh (at most two
    // triangles shared per edge).
    int Clip (const Plane3<Real>& plane);

    // Convert back to a convex polyhedron.
    void Convert (ConvexPolyhedron<Real>& polyhedron);

    // For debugging.
    bool Print (const char* filename) const;

protected:
    // Support for postprocessing faces.
    class EdgePlus
    {
    public:
        EdgePlus ();
        EdgePlus (int e, const Edge& edge);

        bool operator< (const EdgePlus& edge) const;
        bool operator== (const EdgePlus& edge) const;
        bool operator!= (const EdgePlus& edge) const;

        int E, V0, V1, F0, F1;
    };

    void Postprocess (int f, Face& face);

    bool GetOpenPolyline (Face& face, int& vStart, int& vFinal);
    void OrderVertices (Face& face, std::vector<int>& vOrdered);
    void GetTriangles (std::vector<int>& indices,
        std::vector<Plane3<Real> >& plane);

    std::vector<Vertex> mVertices;
    std::vector<Edge> mEdges;
    std::vector<Face> mFaces;
    Real mEpsilon;
};

typedef ConvexClipper<float> ConvexClipperf;
typedef ConvexClipper<double> ConvexClipperd;

#endif
