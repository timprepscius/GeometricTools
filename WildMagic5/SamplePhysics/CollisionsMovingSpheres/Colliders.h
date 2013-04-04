// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.2.0 (2010/06/21)

#ifndef COLLIDERS_H
#define COLLIDERS_H

#include "Wm5Vector3.h"
using namespace Wm5;

// An implementation of the abstract base class of Section 8.3.1 of "3D Game
// Engine Design, 2nd edition".

class Colliders
{
public:
    // Construciton and destruction.
    Colliders ();
    virtual ~Colliders ();

    // Member access.
    void SetDerivativeTimeStep (float timeStep);
    float GetDerivativeTimeStep () const;
    void SetPseudodistanceThreshold (float threshold);
    float GetPseudodistanceThreshold () const;
    void SetMaxIterations (int maxIterations);
    int GetMaxIterations () const;

    enum CollisionType
    {
        UNKNOWN,
        SEPARATED,
        TOUCHING,
        OVERLAPPING
    };

    // The test-intersection query.
    virtual CollisionType Test (
        float fMaxTime,
        const Vector3f& rkVelocity0,
        const Vector3f& rkVelocity1,
        float& rfContactTime);

    // The find intersection query.
    virtual CollisionType Find (
        float fMaxTime,
        const Vector3f& rkVelocity0,
        const Vector3f& rkVelocity1,
        float& rfContactTime);

    // Return the contact time computed by a call to Test or Find.  If there
    // is no contact, the returned time is MAX_REAL.  If the objects are
    // overlapping, the returned time is 0.0f.
    float GetContactTime () const;

protected:
    virtual float Pseudodistance (float time,  const Vector3f& velocity0,
        const Vector3f& velocity1) const = 0;

    virtual void ComputeContactInformation (CollisionType type, float time,
        const Vector3f& velocity0, const Vector3f& velocity1) = 0;

    virtual float PseudodistanceDerivative (float t0, float f0,
        const Vector3f& velocity0, const Vector3f& velocity1) const;

    virtual CollisionType FastNoIntersection (float maxTime,
        const Vector3f& velocity0, const Vector3f& velocity1, float& f0,
        float& fder0);

    float mDerivativeTimeStep;
    float mInvDerivativeTimeStep;
    float mPseudodistanceThreshold;
    int mMaxIterations;
    float mContactTime;
};

#endif
