// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef ROPE_H
#define ROPE_H

#include "Wm5WindowApplication3.h"
#include "PhysicsModule.h"
using namespace Wm5;


class Rope : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    Rope ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void CreateScene ();
    void CreateSprings ();
    void CreateRope ();
    static float Radial (float) { return 0.025f; }

    // The masses are located at the control points of a spline curve.  The
    // control points are connected by a mass-spring system.
    BSplineCurve3f* mSpline;
    PhysicsModule* mModule;
    void PhysicsTick ();
    void GraphicsTick ();

    // The scene graph.
    NodePtr mScene, mTrnNode;
    WireStatePtr mWireState;
    TubeSurfacePtr mRope;
    Culler mCuller;

    // Controlled frame rate.
    float mLastIdle;
};

WM5_REGISTER_INITIALIZE(Rope);
WM5_REGISTER_TERMINATE(Rope);

#endif
