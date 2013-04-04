// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef MORPHCONTROLLERS_H
#define MORPHCONTROLLERS_H

#include "Wm5WindowApplication3.h"
using namespace Wm5;

class MorphControllers : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    MorphControllers ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);
    virtual bool OnMouseClick (int button, int state, int x, int y,
        unsigned int modifiers);

protected:
    void CreateScene ();
    void LoadTargets ();
    void CreateFace ();
    void CreateController ();

    NodePtr mScene;
    WireStatePtr mWireState;
    TriMeshPtr mFace;
    Culler mCuller;

    // Shared data.
    MaterialPtr mMaterial;
    LightPtr mLight;
    IndexBufferPtr mIBuffer;
    LightDirPerVerEffectPtr mEffect;

    enum
    {
        TARGET_QUANTITY = 5,
        KEY_QUANTITY = 6
    };
    TriMeshPtr mTarget[TARGET_QUANTITY];

    // animation time
    double mBaseTime, mCurrTime;

    // picking
    enum { PICK_MESSAGE_SIZE = 128 };
    int mXPick, mYPick;
    char mPickMessage[PICK_MESSAGE_SIZE];
    bool mPickPending;
    Picker mPicker;

    // Support for user-selected face culling.
    int mCurrentFace;

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(MorphControllers);
WM5_REGISTER_TERMINATE(MorphControllers);

#endif
