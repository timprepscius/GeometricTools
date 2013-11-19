// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef INTERSECTINGBOXES_H
#define INTERSECTINGBOXES_H

#include "Wm5WindowApplication3.h"
using namespace Wm5;

class IntersectingBoxes : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    IntersectingBoxes ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void CreateScene ();
    void ModifyBoxes ();
    void ModifyMesh (int i);
    void PhysicsTick ();
    void GraphicsTick ();

    std::vector<AxisAlignedBox3f> mBoxes;
    BoxManagerf* mManager;
    bool mDoSimulation;
    float mLastIdle;
    float mSize;

    NodePtr mScene;
    VisualEffectInstancePtr mNoIntersectEffect, mIntersectEffect;
    WireStatePtr mWireState;
    Culler mCuller;
};

WM5_REGISTER_INITIALIZE(IntersectingBoxes);
WM5_REGISTER_TERMINATE(IntersectingBoxes);

#endif
