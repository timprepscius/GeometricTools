// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.2.0 (2010/06/21)

#ifndef RTSPHERETRIANGLE_H
#define RTSPHERETRIANGLE_H

#include "Wm5Vector3.h"
using namespace Wm5;

// Compute the first time of contact and the corresponding first point of
// contact for a moving sphere and a moving triangle.

struct SphereStruct
{
    Vector3f C;
    float R;
    float RSqr;

    // Call this after R is modified.
    inline void ComputeDerived ()
    {
        RSqr = R*R;
    }
};

struct TriangleStruct
{
    Vector3f P[3];   // vertex positions
    Vector3f E[3];   // edge directions (unit length)
    float H[3];      // edge half-lengths
    Vector3f M[3];   // midpoints of edges
    Vector3f N;      // triangle normal
    Vector3f EN[3];  // edge normals (unit length, outer pointing)

    // Call this after any of P0, P1, or P2 have been modified.
    inline void ComputeDerived ()
    {
        E[0] = P[1] - P[0];
        E[1] = P[2] - P[1];
        E[2] = P[0] - P[2];
        H[0] = 0.5f*E[0].Normalize();
        H[1] = 0.5f*E[1].Normalize();
        H[2] = 0.5f*E[2].Normalize();
        M[0] = P[0] + H[0]*E[0];
        M[1] = P[1] + H[1]*E[1];
        M[2] = P[2] + H[2]*E[2];
        N = E[0].UnitCross(E[1]);
        EN[0] = E[0].Cross(N);
        EN[1] = E[1].Cross(N);
        EN[2] = E[2].Cross(N);
    }
};

enum ContactType
{
    SEPARATED,
    CONTACT,
    OVERLAPPING
};

// The time interval of the query is [0,tMax].  On return,
//   type = SEPARATED:  No intersection during the time interval, so
//       contactTime and contactPoint are invalid.
//   type = CONTACT:  A single time and point of contact during the
//       time interval, so contactTime and contactPoint are valid.
//   type = OVERLAPPING:  The sphere and triangle overlap at time zero.  The
//       set of intersection has infinitely many points.  The contactTime
//       stores zero.  The contactPoint stores the triangle point that is
//       closest to the sphere center.

ContactType Collide (const SphereStruct& sph, const Vector3f& sphVelocity,
    const TriangleStruct& tri, const Vector3f& triVelocity, float tMax,
    float& contactTime, Vector3f& contactPoint);

#endif
