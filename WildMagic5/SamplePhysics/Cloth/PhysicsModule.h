// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef PHYSICSMODULE_H
#define PHYSICSMODULE_H

#include "Wm5MassSpringSurface.h"
using namespace Wm5;

class PhysicsModule : public MassSpringSurface3f
{
public:
    // Construction and destruction.
    //
    // Gravity is controlled by the input 'gravity'.
    //
    // Mass-spring systems tend to exhibit stiffness (in the sense of
    // numerical stability).  To remedy this problem, a small amount of
    // viscous friction is added to the external force.  This term is
    // of the form -viscosity*velocity where viscosity > 0.
    //
    // The initial wind force is specified by the caller.  The wind remains
    // in effect throughout the simulation.  To simulate oscillatory behavior
    // locally, random forces are applied at each mass in the direction
    // perpendicular to the plane of the wind and gravity vectors.  The
    // amplitudes are sinusoidal.  The phases are randomly generated.

    PhysicsModule (int numRows, int numCols, float step,
        const Vector3f& gravity, const Vector3f& wind, float viscosity,
        float amplitude);

    virtual ~PhysicsModule ();

    // Acceleration is due to forces of gravitation, wind, and viscous
    // friction.  The wind forces are randomly generated.
    virtual Vector3f ExternalAcceleration (int i, float time,
        const Vector3f* positions, const Vector3f* velocities);

protected:
    Vector3f mGravity, mWind;
    float mViscosity;

    // sinusoidal forces
    float mAmplitude;
    float* mPhases;
    Vector3f mDirection;
};

#endif
