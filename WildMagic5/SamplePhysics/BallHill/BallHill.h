// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef BALLHILL_H
#define BALLHILL_H

#include "Wm5WindowApplication3.h"
#include "PhysicsModule.h"
using namespace Wm5;

class BallHill : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    BallHill ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void InitializeModule ();
    void CreateScene ();
    TriMesh* CreateGround ();
    TriMesh* CreateHill ();
    TriMesh* CreateBall ();
    Polysegment* CreatePath ();

    APoint UpdateBall ();
    void PhysicsTick ();
    void GraphicsTick ();

    NodePtr mScene;
    WireStatePtr mWireState;
    Culler mCuller;

    TriMeshPtr mGround;
    TriMeshPtr mHill;

    float mRadius;
    TriMeshPtr mBall;

    // polyline path of ball
    PolysegmentPtr mPath;
    int mNextPoint;

    // physics
    PhysicsModule mModule;

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(BallHill);
WM5_REGISTER_TERMINATE(BallHill);

#endif
