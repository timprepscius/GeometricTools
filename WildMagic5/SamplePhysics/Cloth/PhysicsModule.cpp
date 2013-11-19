// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "PhysicsModule.h"
#include "Wm5Memory.h"

//----------------------------------------------------------------------------
PhysicsModule::PhysicsModule (int numRows, int numCols, float step,
    const Vector3f& gravity, const Vector3f& wind, float viscosity,
    float amplitude)
    :
    MassSpringSurface3f(numRows, numCols, step),
    mGravity(gravity),
    mWind(wind),
    mViscosity(viscosity),
    mAmplitude(amplitude)
{
    mDirection = mGravity.UnitCross(mWind);

    mPhases = new1<float>(mNumParticles);
    for (int row = 0; row < mNumRows; ++row)
    {
        for (int col = 0; col < mNumCols; ++col)
        {
            mPhases[GetIndex(row, col)] = Mathf::UnitRandom()*Mathf::PI;
        }
    }
}
//----------------------------------------------------------------------------
PhysicsModule::~PhysicsModule ()
{
    delete1(mPhases);
}
//----------------------------------------------------------------------------
Vector3f PhysicsModule::ExternalAcceleration (int i, float time,
    const Vector3f*, const Vector3f* velocities)
{
    // Acceleration due to gravity, wind, and viscosity.
    Vector3f acceleration = mGravity + mWind - mViscosity*velocities[i];

    // Add a sinusoidal perturbation.
    float amplitude = mAmplitude*Mathf::Sin(2.0f*time + mPhases[i]);
    acceleration += amplitude*mDirection;

    return acceleration;
}
//----------------------------------------------------------------------------
