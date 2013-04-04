// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef CAMERAANDLIGHTNODES_H
#define CAMERAANDLIGHTNODES_H

#include "Wm5WindowApplication3.h"
using namespace Wm5;

class CameraAndLightNodes : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    CameraAndLightNodes ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);
    virtual void MoveForward ();
    virtual void MoveBackward ();
    virtual void TurnLeft ();
    virtual void TurnRight ();
    virtual void MoveUp ();
    virtual void MoveDown ();
    virtual void LookUp ();
    virtual void LookDown ();

protected:
    void CreateScene ();
    TriMesh* CreateGround ();
    TriMesh* CreateLightTarget (Light* light);
    Node* CreateLightFixture (LightPtr& adjustableLight);
    void CreateScreenPolygon ();

    NodePtr mScene;
    WireStatePtr mWireState;
    CameraNodePtr mCNode;
    CameraPtr mScreenCamera;
    TriMeshPtr mSky;
    LightPtr mAdjustableLight0, mAdjustableLight1;
    Culler mCuller;

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(CameraAndLightNodes);
WM5_REGISTER_TERMINATE(CameraAndLightNodes);

#endif
