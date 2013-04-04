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
{
}
//----------------------------------------------------------------------------
PhysicsModule::~PhysicsModule ()
{
}
//----------------------------------------------------------------------------
void PhysicsModule::Initialize (double time, double deltaTime, double y1,
    double dy1, double dy3)
{
    mTime = time;
    mDeltaTime = deltaTime;

    // derived parameters
    mAlpha = Mass1 + Mass2 + Mass3;
    mBeta = Mass1 - Mass2;
    mGamma = Mass1 + Mass2 + Inertia/(Radius*Radius);
    double tmp = SpringConstant*mGamma/(mAlpha*mGamma - mBeta*mBeta);
    assertion(tmp > 0.0, "Invalid constants.\n");
    mOmega = Mathd::Sqrt(tmp);
    mGDivOmegaSqr = Gravity/(mOmega*mOmega);
    mDelta = mBeta*mOmega*mOmega/mGamma;

    // The initial conditions of the system.
    mY1 = y1;
    mDY1 = dy1;
    mY2 = WireLength - Mathd::PI*Radius - mY1;
    mDY2 = -mDY1;
    mY3 = SpringLength;
    mDY3 = dy3;

    // The solution parameters.
    mLPlusGDivOmegaSqr = SpringLength + mGDivOmegaSqr;
    mK1 = mDY3/mOmega;
    mK2 = mY3 - mLPlusGDivOmegaSqr;
    mTCoeff = mDY1 + mDelta*mK1/mOmega;
    mDeltaDivOmegaSqr = mDelta/(mOmega*mOmega);
    mTSqrCoeff = 0.5*Gravity*mDeltaDivOmegaSqr;

    // The initial values of the solution.
    mY1Curr = mY1;
    mY2Curr = mY2;
    mY3Curr = mY3;
}
//----------------------------------------------------------------------------
void PhysicsModule::Update ()
{
    if (mY1Curr <= mY3Curr || mY2Curr <= mY3Curr)
    {
        // One of the masses has "lodged" in the pulley.  No more motion is
        // allowed.
        return;
    }

    if (mY1Curr >= 255.0 || mY2Curr >= 255.0)
    {
        // One of the masses has reached the floor.  No more motion is
        // allowed.
        return;
    }

    mTime += mDeltaTime;

    double scaledTime = mOmega*mTime;
    double sn = Mathd::Sin(scaledTime);
    double cs = Mathd::Cos(scaledTime);
    double combo = mK1*sn + mK2*cs;

    mY3Curr = combo + mLPlusGDivOmegaSqr;
    mY1Curr = mY1 + mTime*(mTCoeff + mTSqrCoeff*mTime) -
        mDeltaDivOmegaSqr*combo;
    mY2Curr = WireLength - Mathd::PI*Radius - mY1Curr;
}
//----------------------------------------------------------------------------
