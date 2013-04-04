// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef FREETOPFIXEDTIP_H
#define FREETOPFIXEDTIP_H

#include "Wm5WindowApplication3.h"
#include "PhysicsModule.h"
using namespace Wm5;

class FreeTopFixedTip : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    FreeTopFixedTip ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void InitializeModule ();
    void CreateScene ();
    TriMesh* CreateFloor ();
    TriMesh* CreateTop ();
    Polysegment* CreateAxisTop ();
    Polysegment* CreateAxisVertical ();
    void PhysicsTick ();
    void GraphicsTick ();

    // The scene graph.
    NodePtr mScene, mTopRoot;
    WireStatePtr mWireState;
    Culler mCuller;
    float mMaxPhi;

    // The physics system.
    PhysicsModule mModule;

    // Support for clamping the frame rate.
    float mLastIdle;
};

WM5_REGISTER_INITIALIZE(FreeTopFixedTip);
WM5_REGISTER_TERMINATE(FreeTopFixedTip);

#endif
