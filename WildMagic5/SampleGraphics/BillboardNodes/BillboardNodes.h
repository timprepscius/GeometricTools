// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef BILLBOARDNODES_H
#define BILLBOARDNODES_H

#include "Wm5WindowApplication3.h"
using namespace Wm5;

#define DEMONSTRATE_VIEWPORT_BOUNDING_RECTANGLE
#define DEMONSTRATE_POST_PROJECTION_REFLECTION

class BillboardNodes : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    BillboardNodes ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void CreateScene ();

    NodePtr mScene;
    Culler mCuller;

    // Override of shader cull and wireframe state.
    CullStatePtr mCullState;
    WireStatePtr mWireState;

    // ground
    TriMeshPtr mGround;

    // billboard0 (rectangle attached)
    BillboardNodePtr mBillboard0;
    TriMeshPtr mRectangle;

    // billboard1 (torus attached)
    BillboardNodePtr mBillboard1;
    TriMeshPtr mTorus;

#ifdef DEMONSTRATE_VIEWPORT_BOUNDING_RECTANGLE
    CameraPtr mSSCamera;
    TriMeshPtr mSSRectangle;
#endif

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(BillboardNodes);
WM5_REGISTER_TERMINATE(BillboardNodes);

#endif
