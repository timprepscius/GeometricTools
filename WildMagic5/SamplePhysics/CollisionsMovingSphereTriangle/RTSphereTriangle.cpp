// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.2.0 (2010/06/21)

#include "RTSphereTriangle.h"

//----------------------------------------------------------------------------
// Compute the squared distance from Q to the triangle.
//----------------------------------------------------------------------------
static float SqrDistance (Vector3f Q, const TriangleStruct& tri,
    Vector3f& closest)
{
    Vector3f QmP0 = Q - tri.P[0];
    Vector3f QmP1 = Q - tri.P[1];
    Vector3f QmP2 = Q - tri.P[2];
    float dot0 = tri.EN[0].Dot(QmP0);
    float dot1 = tri.EN[1].Dot(QmP1);
    float dot2 = tri.EN[2].Dot(QmP2);
    float E0dQmP0, E0dQmP1, E1dQmP1, E1dQmP2, E2dQmP2, E2dQmP0;

    if (dot0 > 0.0f)
    {
        if (dot1 > 0.0f)
        {
            if (dot2 > 0.0f)
            {
                // +++ (Cannot reach this case.  Not all three edges can be
                // visible from outside the triangle.)
                assertion(false, "Theoreticaly cannot reach this.\n");
                closest = (tri.P[0] + tri.P[1] + tri.P[2])/3.0f;
                Vector3f diff = Q - closest;
                return diff.Dot(diff);
            }
            else
            {
                // ++- (E0 and E1 visible)
                E0dQmP0 = tri.E[0].Dot(QmP0);
                if (E0dQmP0 <= 0.0f)
                {
                    // P0 is closest feature.
                    closest = tri.P[0];
                    return QmP0.Dot(QmP0);
                }

                E0dQmP1 = tri.E[0].Dot(QmP1);
                if (E0dQmP1 < 0.0f)
                {
                    // E0 is closest feature.
                    closest = tri.P[0] + E0dQmP0*tri.E[0];
                    return fabsf(QmP0.Dot(QmP0) - E0dQmP0*E0dQmP0);
                }

                E1dQmP1 = tri.E[1].Dot(QmP1);
                if (E1dQmP1 <= 0.0f)
                {
                    // P1 is closest feature.
                    closest = tri.P[1];
                    return QmP1.Dot(QmP1);
                }

                E1dQmP2 = tri.E[1].Dot(QmP2);
                if (E1dQmP2 < 0.0f)
                {
                    // E1 is closest feature.
                    closest = tri.P[1] + E1dQmP1*tri.E[1];
                    return fabsf(QmP1.Dot(QmP1) - E1dQmP1*E1dQmP1);
                }

                // P2 is closest feature.
                closest = tri.P[2];
                return QmP2.Dot(QmP2);
            }
        }
        else
        {
            if (dot2 > 0.0f)
            {
                // +-+ (E2 and E0 visible)
                E2dQmP2 = tri.E[2].Dot(QmP2);
                if (E2dQmP2 <= 0.0f)
                {
                    // P2 is closest feature.
                    closest = tri.P[2];
                    return QmP2.Dot(QmP2);
                }

                E2dQmP0 = tri.E[2].Dot(QmP0);
                if (E2dQmP0 < 0.0f)
                {
                    // E2 is closest feature.
                    closest = tri.P[2] + E2dQmP2*tri.E[2];
                    return fabsf(QmP2.Dot(QmP2) - E2dQmP2*E2dQmP2);
                }

                E0dQmP0 = tri.E[0].Dot(QmP0);
                if (E0dQmP0 <= 0.0f)
                {
                    // P0 is closest feature.
                    closest = tri.P[0];
                    return QmP0.Dot(QmP0);
                }

                E0dQmP1 = tri.E[0].Dot(QmP1);
                if (E0dQmP1 < 0.0f)
                {
                    // E0 is closest feature.
                    closest = tri.P[0] + E0dQmP0*tri.E[0];
                    return fabsf(QmP0.Dot(QmP0) - E0dQmP0*E0dQmP0);
                }

                // P1 is closest feature.
                closest = tri.P[1];
                return QmP1.Dot(QmP1);
            }
            else
            {
                // +-- (E0 visible)
                E0dQmP0 = tri.E[0].Dot(QmP0);
                if (E0dQmP0 <= 0.0f)
                {
                    // P0 is closest feature.
                    closest = tri.P[0];
                    return QmP0.Dot(QmP0);
                }

                E0dQmP1 = tri.E[0].Dot(QmP1);
                if (E0dQmP1 < 0.0f)
                {
                    // E0 is closest feature.
                    closest = tri.P[0] + E0dQmP0*tri.E[0];
                    return fabsf(QmP0.Dot(QmP0) - E0dQmP0*E0dQmP0);
                }

                // P1 is closest feature.
                closest = tri.P[1];
                return QmP1.Dot(QmP1);
            }
        }
    }
    else
    {
        if (dot1 > 0.0f)
        {
            if (dot2 > 0.0f)
            {
                // -++ (E1 and E2 visible)
                E1dQmP1 = tri.E[1].Dot(QmP1);
                if (E1dQmP1 <= 0.0f)
                {
                    // P1 is closest feature.
                    closest = tri.P[1];
                    return QmP1.Dot(QmP1);
                }

                E1dQmP2 = tri.E[1].Dot(QmP2);
                if (E1dQmP2 < 0.0f)
                {
                    // E1 is closest feature.
                    closest = tri.P[1] + E1dQmP1*tri.E[1];
                    return fabsf(QmP1.Dot(QmP1) - E1dQmP1*E1dQmP1);
                }

                E2dQmP2 = tri.E[2].Dot(QmP2);
                if (E2dQmP2 <= 0.0f)
                {
                    // P2 is closest feature.
                    closest = tri.P[2];
                    return QmP2.Dot(QmP2);
                }

                E2dQmP0 = tri.E[2].Dot(QmP0);
                if (E2dQmP0 < 0.0f)
                {
                    // E2 is closest feature.
                    closest = tri.P[2] + E2dQmP2*tri.E[2];
                    return fabsf(QmP2.Dot(QmP2) - E2dQmP2*E2dQmP2);
                }

                // P0 is closest feature.
                closest = tri.P[0];
                return QmP0.Dot(QmP0);
            }
            else
            {
                // -+- (E1 visible)
                E1dQmP1 = tri.E[1].Dot(QmP1);
                if (E1dQmP1 <= 0.0f)
                {
                    // P1 is closest feature.
                    closest = tri.P[1];
                    return QmP1.Dot(QmP1);
                }

                E1dQmP2 = tri.E[1].Dot(QmP2);
                if (E1dQmP2 < 0.0f)
                {
                    // E1 is closest feature.
                    closest = tri.P[1] + E1dQmP1*tri.E[1];
                    return fabsf(QmP1.Dot(QmP1) - E1dQmP1*E1dQmP1);
                }

                // P2 is closest feature.
                closest = tri.P[2];
                return QmP2.Dot(QmP2);
            }
        }
        else
        {
            if (dot2 > 0.0f)
            {
                // --+ (E2 visible)
                E2dQmP2 = tri.E[2].Dot(QmP2);
                if (E2dQmP2 <= 0.0f)
                {
                    // P2 is closest feature.
                    closest = tri.P[2];
                    return QmP2.Dot(QmP2);
                }

                E2dQmP0 = tri.E[2].Dot(QmP0);
                if (E2dQmP0 < 0.0f)
                {
                    // E2 is closest feature.
                    closest = tri.P[2] + E2dQmP2*tri.E[2];
                    return fabsf(QmP2.Dot(QmP2) - E2dQmP2*E2dQmP2);
                }

                // P0 is closest feature.
                closest = tri.P[0];
                return QmP0.Dot(QmP0);
            }
            else
            {
                // --- (projection of point inside triangle)
                float NdQmP0 = tri.N.Dot(QmP0);
                closest = Q - NdQmP0*tri.N;
                return fabsf(NdQmP0);
            }
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Clip C+t*V with [t0,t1] against the plane Dot(N,X-P) = 0, discarding that
// portion of the interval on the side of the plane to which N is directed.
// The return value is 'true' when a nonempty interval exists after clipping.
//----------------------------------------------------------------------------
static bool ClipAgainstPlane (const Vector3f& C, const Vector3f& V,
    const Vector3f& N, const Vector3f& P, float& t0, float& t1)
{
    // Define f(t) = Dot(N,C+t*V-P)
    //             = Dot(N,C-P) + t*Dot(N,V)
    //             = a0 + t*a1
    // Evaluate at the endpoints of the time interval.
    float a0 = N.Dot(C - P);
    float a1 = N.Dot(V);
    float f0 = a0 + t0*a1;
    float f1 = a0 + t1*a1;

    // Clip [t0,t1] against the plane.  There are nine cases to consider,
    // depending on the signs of f0 and f1.
    if (f0 > 0.0f)
    {
        if (f1 > 0.0f)
        {
            // The segment is strictly outside the plane.
            return false;
        }
        else if (f1 < 0.0f)
        {
            // The segment intersects the plane at an edge-interior point.
            // T = -a0/a1 is the time of intersection, so discard [t0,T].
            t0 = -a0/a1;
        }
        else  // f1 == 0.0f
        {
            // The segment is outside the plane but touches at the
            // t1-endpoint, so discard [t0,t1] (degenerate to a point).
            t0 = t1;
        }
    }
    else if (f0 < 0.0f)
    {
        if (f1 > 0.0f)
        {
            // The segment intersects the plane at an edge-interior point.
            // T = -a0/a1 is the time of intersection, so discard [T,t1].
            t1 = -a0/a1;
        }
    }
    else  // f0 == 0.0f
    {
        if (f1 > 0.0f)
        {
            // The segment is outside the plane but touches at the
            // t0-endpoint, so discard [t0,t1] (degenerate to a point).
            t1 = t0;
        }
    }

    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Compute the intersection of the segment C+t*V, [0,tMax], with the sphere
// |X-P| = r.  The function returns 'true' iff [t0,t1] is a nonempty interval.
//----------------------------------------------------------------------------
static bool IntersectLineSphere (const SphereStruct& sphere,
    const Vector3f& V, const Vector3f& P, const float tMax, float& t0,
    float& t1)
{
    t0 = 0.0f;
    t1 = tMax;

    // Compute the coefficients for the quadratic equation
    // Q(t) = |C+t*V-P|^2 - r^2 = q0 + 2*q1*t + q2*t^2.
    Vector3f CmP = sphere.C - P;
    float q2 = V.Dot(V);  // not zero in this application
    float q1 = V.Dot(CmP);
    float q0 = CmP.Dot(CmP) - sphere.RSqr;
    float discr = q1*q1 - q0*q2;
    if (discr >= 0.0f)
    {
        // Q(t) has two distinct real-valued roots (discr > 0) or one repeated
        // real-valued root (discr == 0).
        float invQ2 = 1.0f/q2;
        float rootDiscr = sqrtf(discr);
        float root0 = (-q1 - rootDiscr)*invQ2;
        float root1 = (-q1 + rootDiscr)*invQ2;

        // Compute the intersection of [0,tMax] with [root0,root1].
        if (t1 < root0 || t0 > root1)
        {
            // The intersection is empty.
            return false;
        }
        if (t1 == root0)
        {
            // The intersection is a single point.
            t0 = root0;
            t1 = root0;
            return true;
        }
        if (t0 == root1)
        {
            // The intersection is a single point.
            t0 = root1;
            t1 = root1;
            return true;
        }

        // Here we know that t1 > root0 and t0 < root1.
        if (t0 < root0)
        {
            t0 = root0;
        }
        if (t1 > root1)
        {
            t1 = root1;
        }
        return true;
    }
    else
    {
        // Q(t) has no real-valued roots.
        return false;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Compute the intersection of the segment C+t*V, [0,tMax], with a finite
// cylinder.  The cylinder has center P, radius r, height h, and axis
// direction U2.  The set {U0,U1,U2} is orthonormal and right-handed.  In the
// coordinate system of the cylinder, a point A = P + x*U0 + y*U1 + z*U2.  To
// be inside the cylinder, x*x + y*y <= r*r and |z| <= h/2.  The function
// returns 'true' iff [t0,t1] is a nonempty interval.
//----------------------------------------------------------------------------
static bool IntersectLineCylinder (const SphereStruct& sph,
    const Vector3f& V, const Vector3f& P, const Vector3f& U0,
    const Vector3f& U1, const Vector3f& U2, float halfHeight,
    const float tMax, float& t0, float& t1)
{
    t0 = 0.0f;
    t1 = tMax;

    // Clip against the plane caps.
    if (!ClipAgainstPlane(sph.C, V, U2, P + halfHeight*U2, t0, t1)
    ||  !ClipAgainstPlane(sph.C, V, -U2, P - halfHeight*U2, t0, t1))
    {
        return false;
    }

    // In cylinder coordinates, C+t*V = P + x(t)*U0 + y(t)*U1 + z(t)*U2,
    // x(t) = Dot(U0,C+t*V-P) = a0 + t*b0, y(t) = Dot(U1,C+t*V-P) = a1 + t*b1
    Vector3f CmP = sph.C - P;
    float a0 = U0.Dot(CmP), b0 = U0.Dot(V);
    float a1 = U1.Dot(CmP), b1 = U1.Dot(V);

    // Clip the segment [t0,t1] against the cylinder wall.
    float x0 = a0 + t0*b0, y0 = a1 + t0*b1, r0Sqr = x0*x0 + y0*y0;
    float x1 = a0 + t1*b0, y1 = a1 + t1*b1, r1Sqr = x1*x1 + y1*y1;
    float rSqr = sph.RSqr;

    // Some case require computing intersections of the segment with the
    // circle of radius r.  This amounts to finding roots for the quadratic
    // Q(t) = x(t)*x(t) + y(t)*y(t) - r*r = q2*t^2 + 2*q1*t + q0, where
    // q0 = a0*a0+b0*b0-r*r, q1 = a0*a1+b0*b1, and q2 = a1*a1+b1*b1.  Compute
    // the coefficients only when needed.
    float q0, q1, q2, T;

    if (r0Sqr > rSqr)
    {
        if (r1Sqr > rSqr)
        {
            q2 = b0*b0 + b1*b1;
            if (q2 > 0.0f)
            {
                q0 = a0*a0 + a1*a1 - rSqr;
                q1 = a0*b0 + a1*b1;
                float discr = q1*q1 - q0*q2;
                if (discr < 0.0f)
                {
                    // The quadratic has no real-valued roots, so the
                    // segment is outside the cylinder.
                    return false;
                }

                float rootDiscr = sqrtf(discr);
                float invQ2 = 1.0f/q2;
                float root0 = (-q1 - rootDiscr)*invQ2;
                float root1 = (-q1 + rootDiscr)*invQ2;

                // We know that (x0,y0) and (x1,y1) are outside the
                // cylinder, so Q(t0) > 0 and Q(t1) > 0.  This reduces
                // the number of cases to analyze for intersection of
                // [t0,t1] and [root0,root1].
                if (t1 < root0 || t0 > root1)
                {
                    // The segment is strictly outside the cylinder.
                    return false;
                }
                else
                {
                    // [t0,t1] strictly contains [root0,root1]
                    t0 = root0;
                    t1 = root1;
                }
            }
            else  // q2 == 0.0f and q1 = 0.0f; that is, Q(t) = q0
            {
                // The segment is degenerate, a point that is outside the
                // cylinder.
                return false;
            }
        }
        else if (r1Sqr < rSqr)
        {
            // Solve nondegenerate quadratic and clip.  There must be a single
            // root T in [t0,t1].  Discard [t0,T].
            q0 = a0*a0 + a1*a1 - rSqr;
            q1 = a0*b0 + a1*b1;
            q2 = b0*b0 + b1*b1;
            t0 = (-q1 - sqrtf(fabsf(q1*q1 - q0*q2)))/q2;
        }
        else // r1Sqr == rSqr
        {
            // The segment intersects the circle at t1.  The other root is
            // necessarily T = -t1-2*q1/q2.  Use it only when T <= t1, in
            // which case discard [t0,T].
            q1 = a0*b0 +a1*b1;
            q2 = b0*b0 + b1*b1;
            T = -t1 - 2.0f*q1/q2;
            t0 = (T < t1 ? T : t1);
        }
    }
    else if (r0Sqr < rSqr)
    {
        if (r1Sqr > rSqr)
        {
            // Solve nondegenerate quadratic and clip.  There must be a single
            // root T in [t0,t1].  Discard [T,t1].
            q0 = a0*a0 + a1*a1 - rSqr;
            q1 = a0*b0 + a1*b1;
            q2 = b0*b0 + b1*b1;
            t1 = (-q1 + sqrtf(fabsf(q1*q1 - q0*q2)))/q2;
        }
        // else:  The segment is inside the cylinder.
    }
    else // r0Sqr == rSqr
    {
        if (r1Sqr > rSqr)
        {
            // The segment intersects the circle at t0.  The other root is
            // necessarily T = -t0-2*q1/q2.  Use it only when T >= t0, in
            // which case discard [T,t1].
            q1 = a0*b0 + a1*b1;
            q2 = b0*b0 + b1*b1;
            T = -t0 - 2.0f*q1/q2;
            t1 = (T > t0 ? T : t0);
        }
        // else:  The segment is inside the cylinder.
    }

    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Compute the intersection of the segment C+t*V, [0,tMax], with the extruded
// triangle whose faces are Dot(N,X-P0) = r, Dot(-N,X-P0) = r,
// Dot(EN0,X-P0) = 0, Dot(EN1,X-P1) = 0, and Dot(EN2,X-P2) = 0.  The function
// returns 'true' iff [t0,t1] is a nonempty interval.
//----------------------------------------------------------------------------
static bool IntersectLinePolyhedron (const SphereStruct& sph,
    const Vector3f& V, const TriangleStruct& tri, const float tMax,
    float& t0, float& t1)
{
    t0 = 0.0f;
    t1 = tMax;

    return
        ClipAgainstPlane(sph.C, V,  tri.N, tri.P[0] + sph.R*tri.N, t0, t1) &&
        ClipAgainstPlane(sph.C, V, -tri.N, tri.P[0] - sph.R*tri.N, t0, t1) &&
        ClipAgainstPlane(sph.C, V, tri.EN[0], tri.P[0], t0, t1) &&
        ClipAgainstPlane(sph.C, V, tri.EN[1], tri.P[1], t0, t1) &&
        ClipAgainstPlane(sph.C, V, tri.EN[2], tri.P[2], t0, t1);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
ContactType Collide (const SphereStruct& sph, const Vector3f& sphVelocity,
    const TriangleStruct& tri, const Vector3f& triVelocity, float tMax,
    float& contactTime, Vector3f& contactPoint)
{
    // Test the sphere-triangle relationship at time zero.
    float sqrDistance = SqrDistance(sph.C, tri, contactPoint);
    if (sqrDistance < sph.RSqr)
    {
        contactTime = 0.0f;
        return OVERLAPPING;
    }
    else if (sqrDistance == sph.RSqr)
    {
        contactTime = 0.0f;
        return CONTACT;
    }

    // The sphere and triangle are initially separated.  Compute the velocity
    // of the sphere relative to triangle, so the triangle is then stationary.
    Vector3f V = sphVelocity - triVelocity;
    if (V == Vector3f::ZERO)
    {
        // The objects are stationary relative to each other.
        return SEPARATED;
    }

    float t0Intr = FLT_MAX, t1Intr = -FLT_MAX;
    float t0, t1;

    if (IntersectLinePolyhedron(sph, V, tri, tMax, t0, t1))
    {
        if (t0 < t0Intr)
        {
            t0Intr = t0;
        }
        if (t1 > t1Intr)
        {
            t1Intr = t1;
        }
    }

    int i;
    for (i = 0; i < 3; ++i)
    {
        if (IntersectLineCylinder(sph, V, tri.M[i], tri.N, tri.EN[i],
            tri.E[i], tri.H[i], tMax, t0, t1))
        {
            if (t0 < t0Intr)
            {
                t0Intr = t0;
            }
            if (t1 > t1Intr)
            {
                t1Intr = t1;
            }
        }
    }

    for (i = 0; i < 3; ++i)
    {
        if (IntersectLineSphere(sph, V, tri.P[i], tMax, t0, t1))
        {
            if (t0 < t0Intr)
            {
                t0Intr = t0;
            }
            if (t1 > t1Intr)
            {
                t1Intr = t1;
            }
        }
    }

    if (t0Intr <= t1Intr)
    {
        contactTime = t0Intr;
        sqrDistance = SqrDistance(sph.C + contactTime*V, tri, contactPoint);
        contactPoint += contactTime*triVelocity;
        return CONTACT;
    }
    else
    {
        return SEPARATED;
    }
}
//----------------------------------------------------------------------------
