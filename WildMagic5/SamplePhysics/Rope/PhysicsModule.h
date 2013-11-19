// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef PHYSICSMODULE_H
#define PHYSICSMODULE_H

#include "Wm5MassSpringCurve.h"
using namespace Wm5;

class PhysicsModule : public MassSpringCurve3f
{
public:
    // Construction and destruction.
    //
    // Gravity is controlled by the input 'gravity'.
    //
    // Mass-spring systems tend to exhibit "stiffness" (in the sense of
    // numerical stability).  To remedy this problem, a small amount of
    // viscous friction is added to the external force.  This term is
    // of the form -viscosity*velocity where viscosity > 0.
    //
    // The initial wind force is specified by the caller.  The application
    // of wind can be enabled/disabled by EnableWind().  The member
    // function EnableWindChange() allows the wind direction to change
    // randomly, but each new direction is nearby the old direction in order
    // to obtain some sense of continuity of direction.  The magnitude of
    // the wind force is constant, the length of the initial force.

    PhysicsModule (int numParticles, float step, const Vector3f& gravity,
        const Vector3f& wind, float windChangeAmplitude, float viscosity);

    bool EnableWind;
    bool EnableWindChange;

    // External acceleration is due to forces of gravitation, wind, and
    // viscous friction.  The wind forces are randomly generated.
    virtual Vector3f ExternalAcceleration (int i, float time,
        const Vector3f* positions, const Vector3f* velocities);

protected:
    Vector3f mGravity, mWind;
    float mWindChangeAmplitude, mViscosity;
};

#endif
