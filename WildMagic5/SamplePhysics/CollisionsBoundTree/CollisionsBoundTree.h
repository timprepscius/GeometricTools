// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2012/07/07)

#ifndef COLLISIONSBOUNDTREE_H
#define COLLISIONSBOUNDTREE_H

#include "Wm5WindowApplication3.h"
using namespace Wm5;

class CollisionsBoundTree : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    CollisionsBoundTree ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void CreateScene ();

    NodePtr mScene;
    TriMeshPtr mCylinder0, mCylinder1;
    WireStatePtr mWireState;
    CullStatePtr mCullState;
    Culler mCuller;

    // The collision system.
    typedef BoundTree<TriMesh,Bound> CTree;
    typedef CollisionRecord<TriMesh,Bound> CRecord;
    typedef CollisionGroup<TriMesh,Bound> CGroup;

    CGroup* mGroup;
    bool Transform (unsigned char key);
    void ResetColors ();
    static void Response (CRecord& record0, int t0, CRecord& record1, int t1,
        Intersector<float,Vector3f>* intersector);
    Float2 mBlueUV, mRedUV, mCyanUV, mYellowUV;

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(CollisionsBoundTree);
WM5_REGISTER_TERMINATE(CollisionsBoundTree);

#endif
