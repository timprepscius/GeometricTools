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
    void Initialize (double time, double deltaTime, double x1, double y1,
        double x2, double y2, double xDot, double yDot, double thetaDot);

    // Access the current state.
    inline double GetTime () const;
    inline double GetDeltaTime () const;
    inline double GetX () const;
    inline double GetXDot () const;
    inline double GetY () const;
    inline double GetYDot () const;
    inline double GetTheta () const;
    inline double GetThetaDot () const;
    void Get (double& x1, double& y1, double& x2, double& y2) const;

    // Apply a single step of the solver.
    void Update ();

    // physical constants // symbols used in the Game Physics book
    double Gravity;               // g
    double Mass1, Mass2;          // m1, m2
    double Friction1, Friction2;  // c1, c2

protected:
    // state and auxiliary variables
    double mTime, mDeltaTime;
    double mLength1, mLength2;
    double mState[6], mAux[7];

    // ODE solver (specific solver assigned in the cpp file)
    OdeSolverd* mSolver;
    static void OdeFunction (double time, const double* state, void* data,
        double* output);
};

#include "PhysicsModule.inl"

#endif
