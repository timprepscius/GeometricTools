// Geometric Tools, LLC
// Copyright (c) 1998-2013
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
    Friction(0.0),
    Angle(0.0),
    mTime(0.0),
    mDeltaTime(0.0),
    mSolver(0)
{
    mState[0] = 0.0;
    mState[1] = 0.0;
    mState[2] = 0.0;
    mState[3] = 0.0;
    mAux[0] = 0.0;
    mAux[1] = 0.0;
}
//----------------------------------------------------------------------------
PhysicsModule::~PhysicsModule ()
{
    delete0(mSolver);
}
//----------------------------------------------------------------------------
void PhysicsModule::Initialize (double time, double deltaTime, double x,
    double w, double xDer, double wDer)
{
    mTime = time;
    mDeltaTime = deltaTime;

    // state variables
    mState[0] = x;
    mState[1] = xDer;
    mState[2] = w;
    mState[3] = wDer;

    // auxiliary variables
    mAux[0] = Friction/Mass;
    mAux[1] = Gravity*Mathd::Sin(Angle);

    // RK4 differential equation solver.  Since m_pkSolver is a base class
    // pointer, you can instead create a solver of whatever class you prefer.
    delete0(mSolver);
    mSolver = new0 OdeRungeKutta4d(4, mDeltaTime, OdeFunction, mAux);
}
//----------------------------------------------------------------------------
void PhysicsModule::Update ()
{
    if (mSolver)
    {
        // Apply a single step of the ODE solver.
        mSolver->Update(mTime, mState, mTime, mState);
    }
}
//----------------------------------------------------------------------------
void PhysicsModule::OdeFunction (double, const double* state, void* data,
    double* output)
{
    double* aux = (double*)data;

    double vLen = Mathd::Sqrt(state[1]*state[1] + state[3]*state[3]);
    double xDerFunction, wDerFunction;
    if (vLen > Mathd::ZERO_TOLERANCE)
    {
        double temp = -aux[0]/vLen;
        xDerFunction = temp*state[1];
        wDerFunction = temp*state[3] - aux[1];
    }
    else
    {
        // Velocity is effectively zero, so frictional force is zero.
        xDerFunction = 0.0;
        wDerFunction = -aux[1];
    }

    // x function
    output[0] = state[1];

    // dot(x) function
    output[1] = xDerFunction;

    // w function
    output[2] = state[3];

    // dot(w) function
    output[3] = wDerFunction;
}
//----------------------------------------------------------------------------
