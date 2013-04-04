// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef BOUNCINGBALL_H
#define BOUNCINGBALL_H

#include "Wm5WindowApplication3.h"
#include "DeformableBall.h"
using namespace Wm5;

class BouncingBall : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    BouncingBall ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void CreateScene ();
    void CreateBall ();
    void CreateFloor ();
    void CreateWall ();

    void PhysicsTick ();
    void GraphicsTick ();

    // Representation of body
    DeformableBall* mBall;
    PlanarReflectionEffectPtr mPREffect;

    // Simulated clock.
    float mSimTime, mSimDelta;

    // The scene graph.
    NodePtr mScene, mBallNode;
    TriMeshPtr mFloor, mWall;
    WireStatePtr mWireState;
    VisibleSet mSceneVisibleSet, mBallNodeVisibleSet;

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(BouncingBall);
WM5_REGISTER_TERMINATE(BouncingBall);

#endif
