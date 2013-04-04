// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef ROUGHPLANEPARTICLE1_H
#define ROUGHPLANEPARTICLE1_H

#include "Wm5WindowApplication2.h"
#include "PhysicsModule.h"
using namespace Wm5;

class RoughPlaneParticle1 : public WindowApplication2
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    RoughPlaneParticle1 ();

    virtual bool OnInitialize ();
    virtual void OnIdle ();
    virtual void OnDisplay ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    PhysicsModule mModule;
    std::vector<Vector2d> mSFPositions;  // path with static friction
    bool mContinueSolving;

    // viscous solution:
    //   x(t) = a0*exp(-r*t)+a1
    //   w(t) = b0*exp(-r*t)+b1*t+b2
    //   r = c/m
    //   a0 = -xdot(0)/r
    //   a1 = x(0)-a0
    //   b1 = -g*sin(phi)/r
    //   b2 = (wdot(0)+r*w(0)-b1)/r
    //   b0 = w(0)-b2
    Vector2d GetVFPosition (double dTime);
    double mR, mA0, mA1, mB0, mB1, mB2;
    std::vector<Vector2d> mVFPositions;  // path with viscous friction

    int mSize;
};

WM5_REGISTER_INITIALIZE(RoughPlaneParticle1);
WM5_REGISTER_TERMINATE(RoughPlaneParticle1);

#endif
