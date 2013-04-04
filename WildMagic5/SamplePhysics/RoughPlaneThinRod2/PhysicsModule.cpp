// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "PhysicsModule.h"
#include "Wm5Math.h"
using namespace Wm5;

//----------------------------------------------------------------------------
PhysicsModule::PhysicsModule ()
    :
    MuGravity(0.0),
    Length(0.0),
    mTime(0.0),
    mDeltaTime(0.0),
    mX(0.0),
    mY(0.0),
    mTheta(0.0),
    mXDer(0.0),
    mYDer(0.0),
    mThetaDer(0.0),
    mX0(0.0),
    mY0(0.0),
    mTheta0(0.0),
    mXDer0(0.0),
    mYDer0(0.0),
    mThetaDer0(0.0),
    mHalfLength(0.0),
    mLinVelCoeff(0.0),
    mAngVelCoeff(0.0)
{
}
//----------------------------------------------------------------------------
PhysicsModule::~PhysicsModule ()
{
}
//----------------------------------------------------------------------------
void PhysicsModule::Initialize (double time, double deltaTime, double x,
    double y, double theta, double xDer, double yDer, double thetaDer)
{
    mTime = time;
    mDeltaTime = deltaTime;
    mX = x;
    mY = y;
    mTheta = theta;
    mXDer = xDer;
    mYDer = yDer;
    mThetaDer = thetaDer;

    mX0 = mX;
    mY0 = mY;
    mTheta0 = mTheta;
    mXDer0 = mXDer;
    mYDer0 = mYDer;
    mThetaDer0 = mThetaDer;

    double linSpeed = Mathd::Sqrt(xDer*xDer + yDer*yDer);
    if (linSpeed > Mathd::ZERO_TOLERANCE)
    {
        mLinVelCoeff = MuGravity/linSpeed;
    }
    else
    {
        mLinVelCoeff = 0.0;
    }

    double angSpeed = Mathd::FAbs(thetaDer);
    if (angSpeed > Mathd::ZERO_TOLERANCE)
    {
        mAngVelCoeff = 3.0*MuGravity/(angSpeed*Length);
    }
    else
    {
        mAngVelCoeff = 0.0;
    }

    mHalfLength = 0.5*Length;
}
//----------------------------------------------------------------------------
void PhysicsModule::Get (double& x1, double& y1, double& x2, double& y2) const
{
    double cs = Mathd::Cos(mTheta);
    double sn = Mathd::Sin(mTheta);
    x1 = mX + mHalfLength*cs;
    y1 = mY + mHalfLength*sn;
    x2 = mX - mHalfLength*cs;
    y2 = mY - mHalfLength*sn;
}
//----------------------------------------------------------------------------
void PhysicsModule::Update ()
{
    mTime += mDeltaTime;

    double linTmp = mLinVelCoeff*mTime;
    double linVelMult = 1.0 - linTmp;
    if (linVelMult > 0.0)
    {
        mXDer = linVelMult*mXDer0;
        mYDer = linVelMult*mYDer0;
        mX = mX0 + mTime*(1.0 - 0.5*linTmp)*mXDer0;
        mY = mY0 + mTime*(1.0 - 0.5*linTmp)*mYDer0;
    }
    else
    {
        mXDer = 0.0;
        mYDer = 0.0;
    }

    double angTmp = mAngVelCoeff*mTime;
    double angVelMult = 1.0 - angTmp;
    if (angVelMult > 0.0)
    {
        mThetaDer = angVelMult*mThetaDer0;
        mTheta = mTheta0 + mTime*(1.0 - 0.5*angTmp)*mThetaDer0;
    }
    else
    {
        mThetaDer = 0.0;
    }
}
//----------------------------------------------------------------------------
