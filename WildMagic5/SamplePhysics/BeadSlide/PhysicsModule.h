// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef PHYSICSMODULE_H
#define PHYSICSMODULE_H

#include "Wm5OdeSolver.h"
using namespace Wm5;

class PhysicsModule
{
public:
    // Construction and destruction.
    PhysicsModule ();
    ~PhysicsModule ();

    // Initialize the differential equation solver.
    void Initialize (double time, double deltaTime, double q, double qDot);

    // Take a single step of the solver.
    void Update ();

    // Access the current state.
    inline double GetTime () const;
    inline double GetDeltaTime () const;
    inline double GetQ () const;
    inline double GetQDot () const;

    // Physical constants.
    double Gravity;
    double Mass;

private:
    // State and auxiliary variables.
    double mTime, mDeltaTime, mState[2], mAux[1];

    // ODE solver (specific solver assigned in the cpp file).
    OdeSolverd* mSolver;
    static void OdeFunction (double time, const double* input, void* data,
        double* output);
};

#include "PhysicsModule.inl"

#endif
