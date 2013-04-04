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
#include "Wm5HMatrix.h"
using namespace Wm5;

class PhysicsModule
{
public:
    // Construction and destruction.
    PhysicsModule ();
    ~PhysicsModule ();

    // Initialize the differential equation solver.  The inputs theta, phi,
    // and psi determine the body coordinate axes Xi1, Xi2, and Xi3.  The
    // angular velocity inputs are the coefficients in the body coordinate
    // system.
    void Initialize (double time, double deltaTime, double theta, double phi,
        double psi, double angVel1, double angVel2, double angVel3);

    // Apply a single step of the solver.
    void Update ();

    // Access the current state.
    inline double GetTime () const;
    inline double GetDeltaTime () const;
    inline double GetTheta () const;
    inline double GetPhi () const;
    inline double GetPsi () const;

    // Get the body coordinate axes in world coordinates.  The axes are
    // stored as the columns of a rotation matrix.
    HMatrix GetBodyAxes () const;

    // The physical constants.
    double Gravity;
    double Mass;
    double Length;
    double Inertia1, Inertia3;  // Inertia2 = Inertia1

private:
    // State and auxiliary variables.
    double mTime, mDeltaTime;
    double mState[3], mAux[5];

    // ODE solver (specific solver assigned in the cpp file)
    OdeSolverd* mSolver;
    static void OdeFunction (double time, const double* state, void* data,
        double* output);
};

#include "PhysicsModule.inl"

#endif
