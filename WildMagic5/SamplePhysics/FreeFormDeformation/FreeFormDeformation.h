// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef FREEFORMDEFORMATION_H
#define FREEFORMDEFORMATION_H

#include "Wm5WindowApplication3.h"
#include "Wm5BSplineVolume.h"
using namespace Wm5;

class FreeFormDeformation : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    FreeFormDeformation ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);
    virtual bool OnMouseClick (int button, int state, int x, int y,
        unsigned int modifiers);
    virtual bool OnMotion (int button, int x, int y, unsigned int modifiers);

protected:
    void CreateScene ();
    void CreateBSplineVolume ();
    void CreatePolylines ();
    void CreateControlBoxes ();

    void UpdateMesh ();
    void UpdatePolysegments ();
    void UpdateControlBoxes ();

    void DoRandomControlPoints ();
    void OnMouseDown (int x, int y);
    void OnMouseMove (int x, int y);

    // The scene graph.
    NodePtr mScene, mTrnNode;
    WireStatePtr mWireState;
    TriMeshPtr mMesh;
    Culler mCuller;
    Picker mPicker;

    // The control volume for deformation.
    int mQuantity, mDegree;
    BSplineVolumef* mVolume;
    float mXMin, mYMin, mZMin, mDX, mDY, mDZ;
    Vector3f* mParameters;  // (u,v,w) for mesh vertices

    // Q control points per dimension, 3*Q^2*(Q-1) polysegments to connect
    // them.
    NodePtr mPolysegmentRoot;

    // Toggle between automated random motion and user-adjusted controls.
    bool mDoRandom;

    // Random motion parameters.
    float mAmplitude, mRadius, mLastUpdateTime;

    // User-adjusted controls.
    NodePtr mControlRoot;
    TriMeshPtr mSelected;
    VisualEffectInstancePtr mControlActive;
    VisualEffectInstancePtr mControlInactive;
    APoint mOldWorldPos;
    bool mMouseDown;

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(FreeFormDeformation);
WM5_REGISTER_TERMINATE(FreeFormDeformation);

#endif
