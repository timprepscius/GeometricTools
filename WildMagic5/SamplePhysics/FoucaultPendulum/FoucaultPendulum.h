// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef FOUCAULTPENDULUM_H
#define FOUCAULTPENDULUM_H

#include "Wm5WindowApplication3.h"
#include "PhysicsModule.h"
using namespace Wm5;

class FoucaultPendulum : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    FoucaultPendulum ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void CreateScene ();
    TriMesh* CreateFloor ();
    Polypoint* CreatePath ();
    Node* CreatePendulum ();
    void PhysicsTick ();
    void GraphicsTick ();

    // The scene graph.
    NodePtr mScene, mPendulum;
    WireStatePtr mWireState;
    PolypointPtr mPath;
    int mNextPoint;
    float mColorDiff;
    Culler mCuller;

    // The physics system for the Foucault pendulum.
    PhysicsModule mModule;

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(FoucaultPendulum);
WM5_REGISTER_TERMINATE(FoucaultPendulum);

#endif
