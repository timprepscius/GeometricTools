// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef PHYSICSMODULE_H
#define PHYSICSMODULE_H

#include "Wm5Vector2.h"
using namespace Wm5;

class PhysicsModule
{
public:
    // Construction and destruction.
    PhysicsModule ();
    ~PhysicsModule ();

    // Initialize the system.
    void Initialize (double time, double deltaTime,
        const Vector2d& initialPosition, const Vector2d& initialVelocity);

    // Apply a single step of the simulation.
    void Update ();

    // Access the current state.
    inline double GetTime () const;
    inline double GetDeltaTime () const;
    inline const Vector2d& GetPosition () const;
    inline const Vector2d& GetVelocity () const;
    inline double GetFrequency () const;

    // Physical constants.
    double SpringConstant;  // c
    double Mass;  // m

private:
    void Evaluate ();

    // State variables.
    double mTime, mDeltaTime;
    Vector2d mPosition, mVelocity;

    // Auxiliary variables.
    Vector2d mInitialPosition;
    double mFrequency;  // sqrt(c/m)
    Vector2d mVelDivFreq;  // initialVelocity/frequency
};

#include "PhysicsModule.inl"

#endif
