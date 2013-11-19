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
#include "Wm5HMatrix.h"
using namespace Wm5;

class PhysicsModule
{
public:
    // Construction and destruction.
    PhysicsModule ();
    ~PhysicsModule ();

    // Initialize the differential equation solver.
    void Initialize (double time, double deltaTime, double y1, double y2,
        double y1Dot, double y2Dot);

    // Access the current state.
    inline double GetTime () const;
    inline double GetDeltaTime () const;
    inline double GetY1 () const;
    inline double GetY1Dot () const;
    inline double GetY2 () const;
    inline double GetY2Dot () const;

    // Get the ball center and incremental rotation (after update).
    void GetData (APoint& center, HMatrix& incrRot) const;

    // Compute paraboloid height from an xy-plane position.
    float GetHeight (float x, float y) const;

    // Take a single step of the solver.
    void Update ();

    // The physical constants.
    double Gravity;

    // The paraboloid parameters.
    double A1, A2, A3;

    // The ball radius.
    double Radius;

private:
    // The paraboloid is x3 = a3 - (x1/a1)^2 - (x2/a2)^2.  The equations of
    // motion are:
    //   x1"+(4*x1/a1^2)*((x1*x1"+(x1')^2)/a1^2+(x2*x2"+(x2')^2)/a2^2)
    //     = 2*g*x1/a1^2
    //   x2"+(4*x2/a2^2)*((x1*x1"+(x1')^2)/a1^2+(x2*x2"+(x2')^2)/a2^2)
    //     = 2*g*x2/a2^2
    // Make the change of variables y1 = x1/a2 and y2 = x2/a2.  The equations
    // of motion are:
    //   a1^2*y1"+4*y1*(y1*y1"+(y1')^2+y2*y2"+(y2')^2) = 2g*y1
    //   a2^2*y2"+4*y2*(y1*y1"+(y1')^2+y2*y2"+(y2')^2) = 2g*y2
    // The second derivatives y1" and y2" can be solved algebraically:
    //  +   +   +                       +^{-1} +                             +
    //  |y1"| = |a1^2+4*y1^2 4*y1*y2    |      |2*g*y1-4*y1*((y1')^2+(y2')^2)|
    //  |y2"|   |4*y1*y2     a2^2+4*y2^2|      |2*g*y2-4*y2*((y1')^2+(y2')^2)|
    //  +   +   +                       +      +                             +
    //
    // The four state variables for the RK4 solver.
    //
    // state[0] = y1
    // state[1] = y1'
    // state[2] = y2
    // state[3] = y2'
    //
    // Auxiliary variables that the caller of the RK4 Update function must
    // set before passing to the update.
    //
    // aux[0] = a1^2
    // aux[1] = a2^2
    // aux[2] = g
    // aux[3] = storage for return value of DY2Dot, calculated in DY1Dot

    // State and auxiliary variables.
    double mTime, mDeltaTime, mState[4], mAux[3];

    // ODE solver (specific solver assigned in the cpp file).
    OdeSolverd* mSolver;
    static void OdeFunction (double time, const double* state, void* data,
        double* output);
};

#include "PhysicsModule.inl"

#endif
