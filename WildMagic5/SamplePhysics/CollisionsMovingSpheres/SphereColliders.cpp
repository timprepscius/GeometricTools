// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.2.1 (2012/07/07)

#include "SphereColliders.h"

//----------------------------------------------------------------------------
SphereColliders::SphereColliders (const Sphere3f& sphere0,
    const Sphere3f& sphere1)
    :
    mSphere0(&sphere0),
    mSphere1(&sphere1),
    mContactPoint(Vector3f(Mathf::MAX_REAL, Mathf::MAX_REAL, Mathf::MAX_REAL))
{
}
//----------------------------------------------------------------------------
SphereColliders::~SphereColliders ()
{
}
//----------------------------------------------------------------------------
Vector3f SphereColliders::GetContactPoint () const
{
    return mContactPoint;
}
//----------------------------------------------------------------------------
float SphereColliders::Pseudodistance (float time, const Vector3f& velocity0,
    const Vector3f& velocity1) const
{
    Vector3f movedCenter0 = mSphere0->Center + time*velocity0;
    Vector3f movedCenter1 = mSphere1->Center + time*velocity1;
    Vector3f diff = movedCenter1 - movedCenter0;
    float distance = diff.SquaredLength();
    float rSum = mSphere0->Radius + mSphere1->Radius;
    float pseudodistance = distance/(rSum*rSum) - 1.0f;
    return pseudodistance;
}
//----------------------------------------------------------------------------
void SphereColliders::ComputeContactInformation (CollisionType type,
    float time, const Vector3f&, const Vector3f&)
{
    if (type == SEPARATED)
    {
        float infinity = Mathf::MAX_REAL;
        mContactTime = infinity;
        mContactPoint = Vector3f(infinity, infinity, infinity);
    }
    else if (type == TOUCHING)
    {
        mContactTime = time;
        Vector3f diff = mSphere1->Center - mSphere0->Center;
        diff.Normalize();
        mContactPoint = mSphere0->Center + mSphere0->Radius*diff;
    }
    else if (type == OVERLAPPING)
    {
        // Just return the midpoint of the line segment connecting centers.
        // The actual contact set is either a circle, or one sphere is
        // contained in the other sphere.
        mContactTime = 0.0f;
        mContactPoint = 0.5f*(mSphere0->Center + mSphere1->Center);
    }
    else
    {
        assertion(false, "The type cannot be UNKNOWN.\n");
    }
}
//----------------------------------------------------------------------------
