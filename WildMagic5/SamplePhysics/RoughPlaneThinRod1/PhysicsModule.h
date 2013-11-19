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
    void Initialize (double time, double deltaTime, double x, double y,
        double theta, double xDot, double yDot, double thetaDot);

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

    // physical constants   // symbols used in the Game Physics book
    double Length;          // L1 = L2 = L/2
    double MassDensity;     // delta0
    double Friction;        // c

protected:
    // state and auxiliary variables
    double mTime, mDeltaTime;
    double mState[6], mAux[9];
    double mHalfLength;

    // Integrands for the generalized forces.
    static double FXIntegrand (double ell, void* data);
    static double FYIntegrand (double ell, void* data);
    static double FThetaIntegrand (double ell, void* data);

    // ODE solver (specific solver assigned in the cpp file)
    OdeSolverd* mSolver;
    static void OdeFunction (double time, const double* state, void* data,
        double* output);

    // Romberg integration parameter.
    static int msOrder;
};

#include "PhysicsModule.inl"

#endif
