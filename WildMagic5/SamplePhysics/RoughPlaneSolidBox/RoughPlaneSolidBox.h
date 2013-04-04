// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef ROUGHPLANESOLIDBOX_H
#define ROUGHPLANESOLIDBOX_H

#include "Wm5WindowApplication3.h"
#include "PhysicsModule.h"
using namespace Wm5;

class RoughPlaneSolidBox : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    RoughPlaneSolidBox ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void CreateScene ();
    Node* CreateBox ();
    TriMesh* CreateRamp ();
    TriMesh* CreateGround();
    void InitializeModule ();
    void MoveBox ();
    void PhysicsTick ();
    void GraphicsTick ();

    // The scene graph.
    NodePtr mScene, mBox;
    WireStatePtr mWireState;
    Culler mCuller;

    // The physics system.
    PhysicsModule mModule;
    bool mDoUpdate;

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(RoughPlaneSolidBox);
WM5_REGISTER_TERMINATE(RoughPlaneSolidBox);

#endif
