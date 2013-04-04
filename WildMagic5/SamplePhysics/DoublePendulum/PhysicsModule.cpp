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
    Mass1(0.0),
    Mass2(0.0),
    Length1(0.0),
    Length2(0.0),
    JointX(0.0),
    JointY(0.0),
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
    mAux[2] = 0.0;
    mAux[3] = 0.0;
}
//----------------------------------------------------------------------------
PhysicsModule::~PhysicsModule ()
{
    delete0(mSolver);
}
//----------------------------------------------------------------------------
void PhysicsModule::Initialize (double time, double deltaTime, double theta1,
    double theta2, double theta1Dot, double theta2Dot)
{
    mTime = time;
    mDeltaTime = deltaTime;

    // state variables
    mState[0] = theta1;
    mState[1] = theta1Dot;
    mState[2] = theta2;
    mState[3] = theta2Dot;

    // auxiliary variables
    mAux[0] = Gravity;
    mAux[1] = Length1;
    mAux[2] = Length2;
    mAux[3] = Mass2/(Mass1 + Mass2);

    // RK4 differential equation solver.  Since mSolver is a base class
    // pointer, you can instead create a solver of whatever class you prefer.
    delete0(mSolver);
    mSolver = new0 OdeRungeKutta4d(4, mDeltaTime, OdeFunction, mAux);
}
//----------------------------------------------------------------------------
void PhysicsModule::GetPositions (double& x1, double& y1, double& x2,
    double& y2) const
{
    x1 = JointX + Length1*Mathd::Sin(mState[0]);
    y1 = JointY - Length1*Mathd::Cos(mState[0]);
    x2 = x1 + Length2*Mathd::Sin(mState[2]);
    y2 = y1 - Length2*Mathd::Cos(mState[2]);
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
void PhysicsModule::OdeFunction (double, const double* state,
    void* data, double* output)
{
    double* aux = (double*)data;

    double angleD = state[0] - state[2];
    double csD = Mathd::Cos(angleD);
    double snD = Mathd::Sin(angleD);
    double invDet = 1.0/(aux[1]*aux[2]*(1.0 - aux[3]*csD*csD));
    double sn0 = Mathd::Sin(state[0]);
    double sn2 = Mathd::Sin(state[2]);
    double b1 = -aux[0]*sn0 - aux[3]*aux[2]*snD*state[3]*state[3];
    double b2 = -aux[0]*sn2 + aux[1]*snD*state[1]*state[1];
    double theta1DotFunction = (b1 - aux[3]*csD*b2)*aux[2]*invDet;
    double theta2DotFunction = (b2 - csD*b1)*aux[1]*invDet;

    // theta1 function
    output[0] = state[1];

    // dot(theta1) function
    output[1] = theta1DotFunction;

    // theta2 function
    output[2] = state[3];

    // dot(theta2) function
    output[3] = theta2DotFunction;
}
//----------------------------------------------------------------------------
