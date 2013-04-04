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
    Mu(0.0),
    Gravity(0.0),
    Angle(0.0),
    SinAngle(0.0),
    CosAngle(0.0),
    XLocExt(0.0),
    YLocExt(0.0),
    ZLocExt(0.0),
    mTime(0.0),
    mDeltaTime(0.0),
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
}
//----------------------------------------------------------------------------
PhysicsModule::~PhysicsModule ()
{
    delete0(mSolver);
}
//----------------------------------------------------------------------------
void PhysicsModule::Initialize (double time, double deltaTime, double x,
    double w, double theta, double xDot, double wDot, double thetaDot)
{
    mTime = time;
    mDeltaTime = deltaTime;

    // state variables
    mState[0] = x;
    mState[1] = xDot;
    mState[2] = w;
    mState[3] = wDot;
    mState[4] = theta;
    mState[5] = thetaDot;

    // auxiliary variables
    SinAngle = Mathd::Sin(Angle);
    CosAngle = Mathd::Cos(Angle);
    mAux[0] = Mu*Gravity;  // c/m in the one-particle system example
    mAux[1] = Gravity*SinAngle;

    // RK4 differential equation solver.  Since mSolver is a base class
    // pointer, you can instead create a solver of whatever class you prefer.
    delete0(mSolver);
    mSolver = new0 OdeRungeKutta4d(4, mDeltaTime, OdeFunction, mAux);

    // Set up for angular speed.
    mTheta0 = theta;
    mThetaDer0 = thetaDot;

    double xx = XLocExt*XLocExt;
    double xy = XLocExt*YLocExt;
    double yy = YLocExt*YLocExt;
    double tmp1 = xx + yy;
    double tmp2 = Mathd::Sqrt(tmp1);
    double tmp3 = 4.0*xy/3.0;
    double tmp4 = 0.5*Mathd::Log((tmp2 + XLocExt)/(tmp2 - XLocExt));
    double tmp5 = 0.5*Mathd::Log((tmp2 + YLocExt)/(tmp2 - YLocExt));
    double numer = tmp3*tmp2 + XLocExt*xx*tmp5 + YLocExt*yy*tmp4;
    double denom = tmp3*tmp1;
    double coeff = Mu*Gravity*numer/denom;

    double angSpeed = Mathd::FAbs(thetaDot);
    if (angSpeed > Mathd::ZERO_TOLERANCE)
    {
        mAngVelCoeff = coeff/angSpeed;
    }
    else
    {
        mAngVelCoeff = 0.0;
    }
}
//----------------------------------------------------------------------------
void PhysicsModule::GetRectangle (double& x00, double& y00, double& x10,
    double& y10, double& x11, double& y11, double& x01, double& y01) const
{
    // P = (x,y) + sx*XLocExt*(cos(A),sin(A)) + sy*YLocExt*(-sin(A),cos(A))
    // where |sx| = 1 and |sy| = 1 (four choices on sign)

    double cs = Mathd::Cos(mState[4]);
    double sn = Mathd::Sin(mState[4]);

    // sx = -1, sy = -1
    x00 = mState[0] - XLocExt*cs + YLocExt*sn;
    y00 = mState[2] - XLocExt*sn - YLocExt*cs;

    // sx = +1, sy = -1
    x10 = mState[0] + XLocExt*cs + YLocExt*sn;
    y10 = mState[2] + XLocExt*sn - YLocExt*cs;

    // sx = +1, sy = +1
    x11 = mState[0] + XLocExt*cs - YLocExt*sn;
    y11 = mState[2] + XLocExt*sn + YLocExt*cs;

    // sx = -1, sy = +1
    x01 = mState[0] - XLocExt*cs - YLocExt*sn;
    y01 = mState[2] - XLocExt*sn + YLocExt*cs;
}
//----------------------------------------------------------------------------
void PhysicsModule::Update ()
{
    if (mSolver)
    {
        // Apply a single step of the ODE solver.
        mSolver->Update(mTime, mState, mTime, mState);

        // Update for angular speed.
        double angTmp = mAngVelCoeff*mTime;
        double angVelMult = 1.0 - angTmp;
        if (angVelMult > 0.0)
        {
            mState[5] = angVelMult*mThetaDer0;
            mState[4] = mTheta0 + mTime*(1.0 - 0.5*angTmp)*mThetaDer0;
        }
        else
        {
            mState[5] = 0.0;
        }
    }
}
//----------------------------------------------------------------------------
void PhysicsModule::OdeFunction (double, const double* state, void* data,
    double* output)
{
    double* aux = (double*)data;

    double vLen = Mathd::Sqrt(state[1]*state[1] + state[3]*state[3]);
    double xDotFunction, wDotFunction;
    if (vLen > Mathd::ZERO_TOLERANCE)
    {
        double temp = -aux[0]/vLen;
        xDotFunction = temp*state[1];
        wDotFunction = temp*state[3] - aux[1];
    }
    else
    {
        // Velocity is effectively zero, so frictional force is zero.
        xDotFunction = 0.0;
        wDotFunction = -aux[1];
    }

    // x function
    output[0] = state[1];

    // dot(x) function
    output[1] = xDotFunction;

    // w function
    output[2] = state[3];

    // dot(w) function
    output[3] = wDotFunction;
}
//----------------------------------------------------------------------------
