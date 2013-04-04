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
    void Initialize (double time, double deltaTime, double x, double w,
        double theta, double xDer, double wDer, double thetaDer);

    // Access the current state.
    inline double GetTime () const;
    inline double GetDeltaTime () const;
    inline double GetX () const;
    inline double GetXDer () const;
    inline double GetW () const;
    inline double GetWDer () const;
    inline double GetTheta () const;
    inline double GetThetaDer () const;

    // get rectangle corners in counterclockwise order
    void GetRectangle (double& x00, double& y00, double& x10, double& y10,
        double& x11, double& y11, double& x01, double& y01) const;

    // Apply a single step of the solver.
    void Update ();

    // physical constants // symbols used in the Game Physics book
    double Mu;            // mu
    double Gravity;       // g
    double Angle;         // phi
    double SinAngle;      // sin(phi)
    double CosAngle;      // cos(phi)
    double XLocExt;       // a
    double YLocExt;       // b
    double ZLocExt;       // h

protected:
    // state and auxiliary variables
    double mTime, mDeltaTime;
    double mX, mW, mXDer, mWDer;
    double mState[6], mAux[2];

    // Angular speed can be solved in closed form.
    double mTheta0, mThetaDer0, mAngVelCoeff;

    // ODE solver (specific solver assigned in the cpp file)
    OdeSolverd* mSolver;
    static void OdeFunction (double time, const double* state, void* data,
        double* output);
};

#include "PhysicsModule.inl"

#endif
