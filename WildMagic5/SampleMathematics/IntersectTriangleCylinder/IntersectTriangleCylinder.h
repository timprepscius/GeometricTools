// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.5.0 (2010/11/02)

#ifndef INTERSECTTRIANGLECYLINDER_H
#define INTERSECTTRIANGLECYLINDER_H

#include "Wm5WindowApplication3.h"
using namespace Wm5;

class IntersectTriangleCylinder : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    IntersectTriangleCylinder ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void CreateScene ();
    void TestIntersection ();

    NodePtr mScene;
    TriMeshPtr mTMesh, mCMesh;
    CullStatePtr mCullState;
    WireStatePtr mWireState;
    Culler mCuller;
    Float4 mTextColor;

    APoint mTriangleMVertex0, mTriangleMVertex1, mTriangleMVertex2;
    APoint mCylinderMCenter;
    AVector mCylinderMDirection;
    float mCylinderRadius, mCylinderHeight;
};

WM5_REGISTER_INITIALIZE(IntersectTriangleCylinder);
WM5_REGISTER_TERMINATE(IntersectTriangleCylinder);

#endif
