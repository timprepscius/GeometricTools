// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.2.0 (2010/06/21)

#ifndef SPHERECOLLIDERS_H
#define SPHERECOLLIDERS_H

#include "Colliders.h"
#include "Wm5Sphere3.h"

// An implementation of a class derived from Colliders to illustrate
// intersection queries for spheres moving with constant linear velocity.
// The member functions are based on the discussion in Section 8.3.2 of
// "3D Game Engine Design, 2nd edition".

class SphereColliders : public Colliders
{
public:
    // Construction and destruction.
    SphereColliders (const Sphere3f& sphere0, const Sphere3f& sphere1);
    virtual ~SphereColliders ();

    // Call this function after a Test or Find call *and* when
    // GetContactTime() returns a value T such that 0 <= T <= maxTime,
    // where fMaxTime > 0 is the value supplied to the Test or Find call.
    Vector3f GetContactPoint () const;

protected:
    virtual float Pseudodistance (float time, const Vector3f& velocity0,
        const Vector3f& velocity1) const;

    virtual void ComputeContactInformation (CollisionType type, float time,
        const Vector3f& velocity0, const Vector3f& velocity1);

    const Sphere3f* mSphere0;
    const Sphere3f* mSphere1;
    Vector3f mContactPoint;
};

#endif
