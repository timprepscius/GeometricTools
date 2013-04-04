// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.2.0 (2010/06/21)

#ifndef COLLISIONSMOVINGSPHERES_H
#define COLLISIONSMOVINGSPHERES_H

#include "Wm5WindowApplication3.h"
#include "SphereColliders.h"
using namespace Wm5;

// This sample application and source files were written based on a question
// posted to a www.gamedev.net forum regarding the pseudocode of Section 8.3
// of "3D Game Engine Design, 2nd edition".

class CollisionsMovingSpheres : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    CollisionsMovingSpheres ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void CreateScene ();
    void UpdateSpheres ();

    NodePtr mScene;
    Sphere3f mSphere0, mSphere1, mBoundingSphere;
    Vector3f mVelocity0, mVelocity1;
    TriMeshPtr mMesh0, mMesh1;
    Culler mCuller;
    SphereColliders mColliders;
    float mSimTime, mSimDelta;
    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(CollisionsMovingSpheres);
WM5_REGISTER_TERMINATE(CollisionsMovingSpheres);

#endif
