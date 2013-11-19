// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.2.0 (2010/06/21)

#ifndef COLLISIONSMOVINGSPHERETRIANGLE_H
#define COLLISIONSMOVINGSPHERETRIANGLE_H

#include "Wm5WindowApplication3.h"
#include "RTSphereTriangle.h"
using namespace Wm5;

class CollisionsMovingSphereTriangle : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    CollisionsMovingSphereTriangle ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void CreateScene ();
    void Update ();

    NodePtr mScene;
    SphereStruct mSphere;
    TriangleStruct mTriangle;
    Vector3f mSphereVelocity, mTriangleVelocity;
    TriMeshPtr mSphereMesh, mTriangleMesh;
    PolysegmentPtr mCenters;
    TriMeshPtr mContactMesh;
    WireStatePtr mWireState;
    Culler mCuller;
    float mSimTime, mSimDelta;
    float mContactTime;
    Vector3f mContactPoint;
    Vector3f mMTri[3];
    bool mUseInitialCenter;
    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(CollisionsMovingSphereTriangle);
WM5_REGISTER_TERMINATE(CollisionsMovingSphereTriangle);

#endif
