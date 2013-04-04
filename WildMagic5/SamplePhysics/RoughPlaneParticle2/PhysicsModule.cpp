// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "PhysicsModule.h"
#include "Wm5Memory.h"
#include "Wm5OdeRungeKutta4.h"
#include "Wm5Vector2.h"
using namespace Wm5;

//----------------------------------------------------------------------------
PhysicsModule::PhysicsModule ()
    :
    Gravity(0.0),
    Mass1(0.0),
    Mass2(0.0),
    Friction1(0.0),
    Friction2(0.0),
    mTime(0.0),
    mDeltaTime(0.0),
    mLength1(0.0),
    mLength2(0.0),
    mSolver(0)
{
    mState[0] = 0.0;
    mState[1] = 0.0;
    mState[2] = 0.0;
    mState[3] = 0.0;
    mState[4] = 0.0;
    mState[5] = 0.0;
    mAux[0] = 0.0;
    mAux[1] = 0.0;
    mAux[2] = 0.0;
    mAux[3] = 0.0;
    mAux[4] = 0.0;
    mAux[5] = 0.0;
    mAux[6] = 0.0;
}
//----------------------------------------------------------------------------
PhysicsModule::~PhysicsModule ()
{
    delete0(mSolver);
}
//----------------------------------------------------------------------------
void PhysicsModule::Initialize (double time, double deltaTime, double x1,
    double y1, double x2, double y2, double xDot, double yDot,
    double thetaDot)
{
    mTime = time;
    mDeltaTime = deltaTime;

    // Compute length and moments.
    double deltaX = x2 - x1;
    double deltaY = y2 - y1;
    double length = Mathd::Sqrt(deltaX*deltaX + deltaY*deltaY);
    double mu0 = Mass1 + Mass2;
    double invMu0 = 1.0/mu0;
    double weight1 = Mass1*invMu0;
    double weight2 = Mass2*invMu0;
    mLength1 = weight2*length;
    mLength2 = weight1*length;
    double mu2 = Mass1*mLength1*mLength1 + Mass2*mLength2*mLength2;
    double invMu2 = 1.0/mu2;

    // state variables
    mState[0] = weight1*x1 + weight2*x2;
    mState[1] = xDot;
    mState[2] = weight1*y1 + weight2*y2;
    mState[3] = yDot;
    mState[4] = Mathd::ATan2(deltaY, deltaX);
    mState[5] = thetaDot;

    // auxiliary variable
    mAux[0] = Gravity;
    mAux[1] = mLength1;
    mAux[2] = mLength2;
    mAux[3] = -Friction1*invMu0;
    mAux[4] = -Friction2*invMu0;
    mAux[5] = -Friction1*invMu2;
    mAux[6] = -Friction2*invMu2;

    // RK4 differential equation solver.  Since mSolver is a base class
    // pointer, you can instead create a solver of whatever class you prefer.
    delete0(mSolver);
    mSolver = new0 OdeRungeKutta4d(6, mDeltaTime, OdeFunction, mAux);
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
void PhysicsModule::Get (double& x1, double& y1, double& x2, double& y2) const
{
    double cs = Mathd::Cos(mState[4]);
    double sn = Mathd::Sin(mState[4]);
    x1 = mState[0] + mLength1*cs;
    y1 = mState[2] + mLength1*sn;
    x2 = mState[0] - mLength2*cs;
    y2 = mState[2] - mLength2*sn;
}
//----------------------------------------------------------------------------
void PhysicsModule::OdeFunction (double, const double* state, void* data,
    double* output)
{
    double* aux = (double*)data;

    // Solve the entire system here and save the results to be returned
    // in the later functions.

    double cs = Mathd::Cos(state[4]);
    double sn = Mathd::Sin(state[4]);
    double angCos = state[5]*cs;
    double angSin = state[5]*sn;

    // Compute the friction vectors.  The Normalize function will set a
    // vector to zero if its length is smaller than Mathd::ZERO_TOLERANCE.
    Vector2d F1(state[1] - aux[1]*angSin, state[3] + aux[1]*angCos);
    Vector2d F2(state[1] + aux[2]*angSin, state[3] - aux[2]*angCos);
    F1.Normalize();
    F2.Normalize();
    double xDotFunction = aux[3]*F1.X() + aux[4]*F2.X();
    double yDotFunction = aux[3]*F1.Y() + aux[4]*F2.Y();
    double tmp1 = aux[1]*aux[5]*(cs*F1.Y() - sn*F1.X());
    double tmp2 = aux[2]*aux[6]*(sn*F2.X() - cs*F2.Y());
    double thetaDotFunction = tmp1 + tmp2;

    // x function
    output[0] = state[1];

    // dot(x) function
    output[1] = xDotFunction;

    // y function
    output[2] = state[3];

    // dot(y) function
    output[3] = yDotFunction;

    // theta function
    output[4] = state[5];

    // dot(theta) function
    output[5] = thetaDotFunction;
}
//----------------------------------------------------------------------------
