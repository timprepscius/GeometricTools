// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.1.0 (2010/04/14)

#ifndef BLENDEDANIMATIONS_H
#define BLENDEDANIMATIONS_H

#include "Wm5WindowApplication3.h"
#include "BipedManager.h"
using namespace Wm5;

class BlendedAnimations : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    BlendedAnimations ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);
    virtual bool OnSpecialKeyDown (int key, int x, int y);
    virtual bool OnSpecialKeyUp (int key, int x, int y);

protected:
    void CreateScene ();
    void ComputeVisibleSet (Spatial* object);
    void Update ();

    Float4 mTextColor;
    NodePtr mScene;
    TriMeshPtr mFloor;
    WireStatePtr mWireState;
    VisibleSet mVisibleSet;
    BipedManager mManager;
    double mAnimTime, mAnimTimeDelta;
    bool mUpArrowPressed, mShiftPressed;
};

WM5_REGISTER_INITIALIZE(BlendedAnimations);
WM5_REGISTER_TERMINATE(BlendedAnimations);

#endif
