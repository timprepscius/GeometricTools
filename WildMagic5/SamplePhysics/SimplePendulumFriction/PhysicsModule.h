// Geometric Tools, LLC
// Copyright (c) 1998-2012
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
    void Initialize (double time, double deltaTime, double theta,
        double thetaDot);

    // Apply a single step of the solver.
    void Update ();

    // Access the current state.
    inline double GetTime () const;
    inline double GetDeltaTime () const;
    inline double GetTheta () const;
    inline double GetThetaDot () const;

    // pendulum parameters
    double CDivM;  // friction_coefficient / mass
    double GDivL;  // gravity_constant / pendulum_length

private:
    // state and auxiliary variables
    double mTime, mDeltaTime;
    double mState[2], mAux[2];

    // ODE solver (specific solver assigned in the cpp file)
    OdeSolverd* mSolver;
    static void OdeFunction (double time, const double* state, void* data,
        double* output);
};

#include "PhysicsModule.inl"

#endif
