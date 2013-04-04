// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef PHYSICSMODULE_H
#define PHYSICSMODULE_H

#include "Wm5MassSpringVolume.h"
using namespace Wm5;

class PhysicsModule : public MassSpringVolume3f
{
public:
    // Construction and destruction.
    PhysicsModule (int numSlices, int numRows, int numCols, float step,
        float viscosity);

    virtual ~PhysicsModule ();

    float& Viscosity ();

    // External acceleration is due to viscous forces.
    virtual Vector3f ExternalAcceleration (int i, float time,
        const Vector3f* position, const Vector3f* velocities);

protected:
    float mViscosity;
};

#endif
