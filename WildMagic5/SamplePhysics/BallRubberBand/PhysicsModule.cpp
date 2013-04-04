// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "PhysicsModule.h"
#include "Wm5Math.h"

//----------------------------------------------------------------------------
PhysicsModule::PhysicsModule ()
    :
    SpringConstant(0.0),
    Mass(0.0),
    mTime(0.0),
    mDeltaTime(0.0),
    mPosition(Vector2d::ZERO),
    mVelocity(Vector2d::ZERO),
    mInitialPosition(Vector2d::ZERO),
    mFrequency(0),
    mVelDivFreq(Vector2d::ZERO)
{
}
//----------------------------------------------------------------------------
PhysicsModule::~PhysicsModule ()
{
}
//----------------------------------------------------------------------------
void PhysicsModule::Evaluate ()
{
    double angle = mFrequency*mTime;
    double sn = Mathd::Sin(angle);
    double cs = Mathd::Cos(angle);
    mPosition = cs*mInitialPosition + sn*mVelDivFreq;
    mVelocity = (mVelDivFreq*cs - mInitialPosition*sn)*mFrequency;
}
//----------------------------------------------------------------------------
void PhysicsModule::Initialize (double time, double deltaTime,
    const Vector2d& initialPosition, const Vector2d& initialVelocity)
{
    mTime = time;
    mDeltaTime = deltaTime;
    mInitialPosition = initialPosition;
    mFrequency = Mathd::Sqrt(SpringConstant/Mass);
    mVelDivFreq = initialVelocity/mFrequency;

    Evaluate();
}
//----------------------------------------------------------------------------
void PhysicsModule::Update ()
{
    mTime += mDeltaTime;
    Evaluate();
}
//----------------------------------------------------------------------------
