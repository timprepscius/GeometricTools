// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "PhysicsModule.h"

//----------------------------------------------------------------------------
PhysicsModule::PhysicsModule (int numParticles, int numSprings, float step,
    float viscosity)
    :
    MassSpringArbitrary3f(numParticles, numSprings, step),
    mViscosity(viscosity)
{
}
//----------------------------------------------------------------------------
PhysicsModule::~PhysicsModule ()
{
}
//----------------------------------------------------------------------------
Vector3f PhysicsModule::ExternalAcceleration (int i, float, const Vector3f*,
    const Vector3f* velocities)
{
    Vector3f acceleration = -mViscosity*velocities[i];
    return acceleration;
}
//----------------------------------------------------------------------------
