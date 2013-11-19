// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "PhysicsModule.h"

//----------------------------------------------------------------------------
PhysicsModule::PhysicsModule (int numParticles, float step,
    const Vector3f& gravity, const Vector3f& wind, float windChangeAmplitude,
    float viscosity)
    :
    MassSpringCurve3f(numParticles, step),
    EnableWind(false),
    EnableWindChange(false),
    mGravity(gravity),
    mWind(wind),
    mWindChangeAmplitude(windChangeAmplitude),
    mViscosity(viscosity)
{
}
//----------------------------------------------------------------------------
Vector3f PhysicsModule::ExternalAcceleration (int i, float, const Vector3f*,
    const Vector3f* velocities)
{
    // Acceleration due to gravity.
    Vector3f acceleration = mGravity;

    // Acceleration due to wind.
    if (EnableWind)
    {
        if (EnableWindChange)
        {
            // Generate random direction close to last one.
            Vector3f U, V, W = mWind;
            float length = W.Normalize();
            Vector3f::GenerateComplementBasis(U, V, W);
            float uDelta = mWindChangeAmplitude*Mathf::SymmetricRandom();
            float vDelta = mWindChangeAmplitude*Mathf::SymmetricRandom();
            W += uDelta*U + vDelta*V;
            W.Normalize();
            mWind = length*W;
        }
        acceleration += mWind;
    }

    // Add in a friction term.  Otherwise the system tends to be "stiff"
    // (in the numerical stability sense) and leads to oscillatory behavior.
    acceleration -= mViscosity*velocities[i];

    return acceleration;
}
//----------------------------------------------------------------------------
