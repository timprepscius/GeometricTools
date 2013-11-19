// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef INVERSEKINEMATICS_H
#define INVERSEKINEMATICS_H

#include "Wm5WindowApplication3.h"
using namespace Wm5;

class InverseKinematics : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    InverseKinematics ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);
    virtual bool OnMotion (int button, int x, int y, unsigned int modifiers);

protected:
    void CreateScene ();
    TriMesh* CreateCube ();
    Polysegment* CreateRod ();
    TriMesh* CreateGround ();
    void UpdateRod ();
    bool Transform (unsigned char key);

    NodePtr mScene, mIKSystem, mGoal, mJoint0, mJoint1;
    WireStatePtr mWireState;
    PolysegmentPtr mRod;
    VertexColor3Effect* mVCEffect;
    Culler mCuller;

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(InverseKinematics);
WM5_REGISTER_TERMINATE(InverseKinematics);

#endif
