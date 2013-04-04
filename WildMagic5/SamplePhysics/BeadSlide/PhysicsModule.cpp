// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "PhysicsModule.h"
#include "Wm5Math.h"
#include "Wm5Memory.h"
#include "Wm5OdeRungeKutta4.h"

//----------------------------------------------------------------------------
PhysicsModule::PhysicsModule ()
    :
    Gravity(0.0),
    Mass(0.0),
    mTime(0.0),
    mDeltaTime(0.0),
    mSolver(0)
{
    mState[0] = 0.0;
    mState[1] = 0.0;
    mAux[0] = 0.0;
}
//----------------------------------------------------------------------------
PhysicsModule::~PhysicsModule ()
{
    delete0(mSolver);
}
//----------------------------------------------------------------------------
void PhysicsModule::Initialize (double time, double deltaTime, double q,
    double qDot)
{
    mTime = time;
    mDeltaTime = deltaTime;

    // State variables.
    mState[0] = q;
    mState[1] = qDot;

    // Auxiliary variable.
    mAux[0] = Gravity;

    // RK4 differential equation solver.  Since mSolver is a base class
    // pointer, you can instead create a solver of whatever class you prefer.
    delete0(mSolver);
    mSolver = new0 OdeRungeKutta4d(2, mDeltaTime, OdeFunction, mAux);
}
//----------------------------------------------------------------------------
void PhysicsModule::Update ()
{
    // Apply a single step of the ODE solver.
    if (mSolver)
    {
        mSolver->Update(mTime, mState, mTime, mState);
    }
}
//----------------------------------------------------------------------------
void PhysicsModule::OdeFunction (double, const double* input, void* data,
    double* output)
{
    double* aux = (double*)data;

    double qSqr = input[0]*input[0];
    double qDotSqr = input[1]*input[1];
    double numer = -3.0*aux[0]*qSqr - 2.0*input[0]*(2.0 + 9.0*qSqr)*qDotSqr;
    double denom = 1.0 + qSqr*(4.0 + 9.0*qSqr);
    double qDotFunction = numer/denom;

    // q function
    output[0] = input[1];

    // dot(q) function
    output[1] = qDotFunction;
}
//----------------------------------------------------------------------------
