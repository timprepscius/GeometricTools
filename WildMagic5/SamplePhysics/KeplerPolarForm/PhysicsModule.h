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
    void Initialize (double time, double deltaTime, double radius,
        double theta, double radiusDot, double thetaDot);

    // Apply a single step of the solver.
    void Update ();

    // Access the current state.
    inline double GetTime () const;
    inline double GetDeltaTime () const;
    inline double GetTheta () const;
    inline double GetThetaDot () const;
    inline double GetRadius () const;
    inline double GetRadiusDot () const;

    // physical constants
    double Gravity;
    double Mass;

    // ellipse parameters
    inline double GetEccentricity () const;
    inline double GetMajorAxis () const;
    inline double GetMinorAxis () const;
    double GetPeriod () const;

private:
    // state and auxiliary variables
    double mTime, mDeltaTime;
    double mState[4], mAux[5];

    // ellipse parameters
    double mEccentricity, mRho, mMajorAxis, mMinorAxis;

    // ODE solver (specific solver assigned in the cpp file)
    OdeSolverd* mSolver;
    static void OdeFunction (double time, const double* state, void* data,
        double* output);
};

#include "PhysicsModule.inl"

#endif
