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
    XLocExt(0.0),
    YLocExt(0.0),
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
    double coeff = MuGravity*numer/denom;

    double angSpeed = Mathd::FAbs(thetaDer);
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

    double cs = Mathd::Cos(mTheta);
    double sn = Mathd::Sin(mTheta);

    // sx = -1, sy = -1
    x00 = mX - XLocExt*cs + YLocExt*sn;
    y00 = mY - XLocExt*sn - YLocExt*cs;

    // sx = +1, sy = -1
    x10 = mX + XLocExt*cs + YLocExt*sn;
    y10 = mY + XLocExt*sn - YLocExt*cs;

    // sx = +1, sy = +1
    x11 = mX + XLocExt*cs - YLocExt*sn;
    y11 = mY + XLocExt*sn + YLocExt*cs;

    // sx = -1, sy = +1
    x01 = mX - XLocExt*cs - YLocExt*sn;
    y01 = mY - XLocExt*sn + YLocExt*cs;
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
