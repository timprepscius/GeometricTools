// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.2.0 (2010/06/21)

#include "Colliders.h"

//----------------------------------------------------------------------------
Colliders::Colliders ()
{
    SetDerivativeTimeStep(1e-03f);
    SetPseudodistanceThreshold(1e-06f);
    SetMaxIterations(8);
    mContactTime = Mathf::MAX_REAL;
}
//----------------------------------------------------------------------------
Colliders::~Colliders ()
{
}
//----------------------------------------------------------------------------
void Colliders::SetDerivativeTimeStep (float timeStep)
{
    if (timeStep > 0.0f)
    {
        mDerivativeTimeStep = timeStep;
    }
    else
    {
        mDerivativeTimeStep = 1e-03f;
    }

    mInvDerivativeTimeStep = 0.5f/mDerivativeTimeStep;
}
//----------------------------------------------------------------------------
float Colliders::GetDerivativeTimeStep () const
{
    return mDerivativeTimeStep;
}
//----------------------------------------------------------------------------
void Colliders::SetPseudodistanceThreshold (float threshold)
{
    if (threshold >= 0.0f)
    {
        mPseudodistanceThreshold = threshold;
    }
}
//----------------------------------------------------------------------------
float Colliders::GetPseudodistanceThreshold () const
{
    return mPseudodistanceThreshold;
}
//----------------------------------------------------------------------------
void Colliders::SetMaxIterations (int maxIterations)
{
    if (maxIterations > 0)
    {
        mMaxIterations = maxIterations;
    }
}
//----------------------------------------------------------------------------
int Colliders::GetMaxIterations () const
{
    return mMaxIterations;
}
//----------------------------------------------------------------------------
Colliders::CollisionType Colliders::Test (float maxTime,
    const Vector3f& velocity0, const Vector3f& velocity1, float& contactTime)
{
    return Find(maxTime, velocity0, velocity1, contactTime);
}
//----------------------------------------------------------------------------
Colliders::CollisionType Colliders::Find (float maxTime,
    const Vector3f& velocity0, const Vector3f& velocity1, float& contactTime)
{
    float f0, fder0;
    CollisionType type = FastNoIntersection(maxTime, velocity0, velocity1,
        f0, fder0);

    if (type == SEPARATED)
    {
        contactTime = Mathf::MAX_REAL;
        return SEPARATED;
    }
    if (type == TOUCHING)
    {
        contactTime = 0.0f;
        return TOUCHING;
    }

    // Use Newton’s method for root finding when the derivative is calculated
    // but Secant method when the derivative is estimated.
    float t0 = 0.0f;
    for (int i = 1; i <= mMaxIterations; ++i)
    {
        t0 -= f0/fder0;
        if (t0 > maxTime)
        {
            // The objects do not intersect during the specified time
            // interval.
            contactTime = Mathf::MAX_REAL;
            return TOUCHING;
        }
        f0 = Pseudodistance(t0, velocity0, velocity1);
        if (f0 <= mPseudodistanceThreshold)
        {
            contactTime = t0;
            ComputeContactInformation(TOUCHING, contactTime, velocity0,
                velocity1);
            return TOUCHING;
        }
        fder0 = PseudodistanceDerivative(t0, f0, velocity0, velocity1);
        if (fder0 >= 0.0f)
        {
            // The objects are moving apart.
            contactTime = Mathf::MAX_REAL;
            return SEPARATED;
        }
    }

    // Newton’s method failed to converge, but we already tested earlier
    // whether the objects were moving apart or not intersecting during the
    // specified time interval.  To reach here, the number of iterations was
    // not large enough for the desired pseudodistance threshold.  Most
    // likely this occurs when the relative speed is very large and the time
    // step for the derivative estimation needs to be smaller.
    contactTime = t0;
    ComputeContactInformation(TOUCHING, contactTime, velocity0, velocity1);
    return TOUCHING;
}
//----------------------------------------------------------------------------
float Colliders::GetContactTime () const
{
    return mContactTime;
}
//----------------------------------------------------------------------------
float Colliders::PseudodistanceDerivative (float t0, float f0,
    const Vector3f& velocity0, const Vector3f& velocity1) const
{
    float t1 = t0 - mDerivativeTimeStep;
    float f1 = Pseudodistance(t1,velocity0,velocity1);
    float fder0 = (f0 - f1)*mInvDerivativeTimeStep;
    return fder0;
}
//----------------------------------------------------------------------------
Colliders::CollisionType Colliders::FastNoIntersection (float maxTime,
    const Vector3f& velocity0, const Vector3f& velocity1, float& f0,
    float& fder0)
{
    // Analyze the initial configuration of the objects.
    f0 = Pseudodistance(0.0f, velocity0, velocity1);
    fder0 = PseudodistanceDerivative(0.0f, f0, velocity0, velocity1);
    if (f0 <= -mPseudodistanceThreshold)
    {
        // Objects are (significantly) overlapping.
        ComputeContactInformation(OVERLAPPING, 0.0f, velocity0, velocity1);
        return (fder0 >= 0.0f ? SEPARATED : OVERLAPPING);
    }
    if (f0 <= mPseudodistanceThreshold)
    {
        // Objects are (nearly) in tangential contact.
        ComputeContactInformation(TOUCHING, 0.0f, velocity0, velocity1);
        return (fder0 >= 0.0f ? SEPARATED : TOUCHING);
    }

    // The objects are not currently in contact or overlapping.  If the
    // objects are moving apart or the relative velocity between them is
    // zero, they cannot intersect at a later time.
    if (fder0 >= 0.0f || velocity0 == velocity1)
    {
        return SEPARATED;
    }

    // Check if the objects are not intersecting, yet still moving toward each
    // other at maximum time.  If this is the case, the objects do not
    // intersect on the specified time interval.
    float f1 = Pseudodistance(maxTime, velocity0, velocity1);
    if (f1 > 0.0f)
    {
        // Compute or estimate the derivative F’(tmax).
        float fder1 = PseudodistanceDerivative(maxTime, f1, velocity0,
            velocity1);
        if (fder1 < 0.0f)
        {
            // The objects are moving toward each other and do not intersect
            // during the specified time interval.
            return SEPARATED;
        }
    }
    return UNKNOWN;
}
//----------------------------------------------------------------------------
