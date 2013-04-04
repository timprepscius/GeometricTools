// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef CONVEXPOLYHEDRON_H
#define CONVEXPOLYHEDRON_H

#include "MTMesh.h"
#include "Wm5Plane3.h"
#include "Wm5Vector2.h"
using namespace Wm5;

template <typename Real>
class ConvexPolyhedron : public MTMesh
{
public:
    typedef typename std::vector<Vector2<Real> > V2Array;
    typedef typename std::vector<Vector3<Real> > V3Array;
    typedef typename std::vector<Plane3<Real> > PArray;
    typedef typename std::vector<int> IArray;

    // Construction.
    ConvexPolyhedron ();
    ConvexPolyhedron (const V3Array& points, const IArray& indices);
    ConvexPolyhedron (const V3Array& points, const IArray& indices,
        const PArray& planes);
    ConvexPolyhedron (const ConvexPolyhedron& polyhedron);

    void Create (const V3Array& points, const IArray& indices);
    void Create (const V3Array& points, const IArray& indices,
        const PArray& planes);

    // Assignment.
    ConvexPolyhedron& operator= (const ConvexPolyhedron& polyhedron);

    // Read points and planes.
    const V3Array& GetPoints () const;
    const Vector3<Real>& GetPoint (int i) const;
    const PArray& GetPlanes () const;
    const Plane3<Real>& GetPlane (int i) const;

    // Allow vertex modification.  The caller is responsible for preserving
    // the convexity.  After modifying the vertices, call UpdatePlanes to
    // recompute the planes of the polyhedron faces.
    int AddPoint (const Vector3<Real>& point);
    V3Array& Points ();
    Vector3<Real>& Point (int i);
    void UpdatePlanes ();

    // Test for convexity:  Assuming the application has guaranteed that the
    // mesh is manifold and closed, this function will iterate over the faces
    // of the polyhedron and verify for each that the polyhedron vertices are
    // all on the nonnegative side of the plane.  The threshold is the value
    // that the plane distance d is compared to, d < 0.  In theory the
    // distances should all be nonegative.  Floating point round-off errors
    // can cause some small distances, so you might set epsilon to a small
    // negative number.
    bool ValidateHalfSpaceProperty (Real epsilon = (Real)0) const;
    void ComputeCentroid ();
    const Vector3<Real>& GetCentroid () const;

    // Discard the portion of the mesh on the negative side of the plane.
    bool Clip (const Plane3<Real>& plane, ConvexPolyhedron& intersection)
        const;

    // Compute the polyhedron of intersection.
    bool FindIntersection (const ConvexPolyhedron& polyhedron,
        ConvexPolyhedron& intersection) const;

    static void FindAllIntersections (int numPolyhedra,
        ConvexPolyhedron* polyhedra, int& combos,
        ConvexPolyhedron**& intersections);

    Real GetSurfaceArea () const;
    Real GetVolume () const;
    bool ContainsPoint (const Vector3<Real>& point) const;

    // The eye point must be outside the polyhedron.  The output is the
    // terminator, an ordered list of vertices forming a simple closed
    // polyline that separates the visible from invisible faces of the
    // polyhedron.
    void ComputeTerminator (const Vector3<Real>& eye,
        V3Array& terminator);

    // If projection plane is Dot(N,X) = c where N is unit length, then the
    // application must ensure that Dot(N,eye) > c.  That is, the eye point is
    // on the side of the plane to which N points.  The application must also
    // specify two vectors U and V in the projection plane so that {U,V,N} is
    // a right-handed and orthonormal set (the matrix [U V N] is orthonormal
    // with determinant 1).  The origin of the plane is computed internally as
    // the closest point to the eye point (an orthogonal pyramid for the
    // perspective projection).  If all vertices P on the terminator satisfy
    // Dot(N,P) < Dot(N,eye), then the polyhedron is completely visible (in
    // the sense of perspective projection onto the viewing plane).  In this
    // case the silhouette is computed by projecting the terminator points
    // onto the viewing plane.  The return value of the function is 'true'
    // when this happens.  However, if at least one terminator point P
    // satisfies Dot(N,P) >= Dot(N,eye), then the silhouette is unbounded in
    // the view plane.  It is not computed and the function returns 'false'.
    // A silhouette point (x,y) is extracted from the point Q that is the
    // intersection of the ray whose origin is the eye point and that contains
    // a terminator point, Q = K+x*U+y*V+z*N where K is the origin of the
    // plane.
    bool ComputeSilhouette (const Vector3<Real>& eye,
        const Plane3<Real>& plane, const Vector3<Real>& U,
        const Vector3<Real>& V, V2Array& silhouette);

    bool ComputeSilhouette (V3Array& terminator, const Vector3<Real>& eye,
        const Plane3<Real>& plane, const Vector3<Real>& U,
        const Vector3<Real>& V, V2Array& silhouette);

    // Create an egg-shaped object that is axis-aligned and centered at
    // (xc,yc,zc).  The input bounds are all positive and represent the
    // distances from the center to the six extreme points on the egg.
    static void CreateEggShape (const Vector3<Real>& center, Real x0,
        Real x1, Real y0, Real y1, Real z0, Real z1, int maxSteps,
        ConvexPolyhedron& egg);

    // Debugging support.
    virtual void Print (std::ofstream& outFile) const;
    virtual bool Print (const char* filename) const;

protected:
    // Support for intersection testing.
    static ConvexPolyhedron* FindSolidIntersection (
        const ConvexPolyhedron& polyhedron0,
        const ConvexPolyhedron& polyhedron1);

    static int GetHighBit (int i);

    // Support for computing surface area.
    Real GetTriangleArea (const Vector3<Real>& normal,
        const Vector3<Real>& vertex0, const Vector3<Real>& vertex1,
        const Vector3<Real>& vertex2) const;

    // Support for computing the terminator and silhouette.
    Real GetDistance (const Vector3<Real>& eye, int t,
        std::vector<Real>& distances) const;

    static bool IsNegativeProduct (Real distance0, Real distance1);

    V3Array mPoints;
    PArray mPlanes;
    Vector3<Real> mCentroid;
};

typedef ConvexPolyhedron<float> ConvexPolyhedronf;
typedef ConvexPolyhedron<double> ConvexPolyhedrond;

#endif
