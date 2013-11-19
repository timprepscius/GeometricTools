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
    void Initialize (double time, double deltaTime, double theta1,
        double theta2, double theta1Dot, double theta2Dot);

    // Access the current state.
    inline double GetTime () const;
    inline double GetDeltaTime () const;
    inline double GetTheta1 () const;
    inline double GetTheta1Dot () const;
    inline double GetTheta2 () const;
    inline double GetTheta2Dot () const;
    void GetPositions (double& x1, double& y1, double& x2,
        double& y2) const;

    // Apply a single step of the solver.
    void Update ();

    // The physical constants.
    double Gravity;
    double Mass1, Mass2;
    double Length1, Length2;
    double JointX, JointY;

protected:
    // State and auxiliary variables.
    double mTime, mDeltaTime;
    double mState[4], mAux[4];

    // ODE solver (specific solver assigned in the cpp file).
    OdeSolverd* mSolver;
    static void OdeFunction (double time, const double* state, void* data,
        double* output);
};

#include "PhysicsModule.inl"

#endif
