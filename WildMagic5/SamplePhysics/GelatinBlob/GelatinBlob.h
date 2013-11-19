// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef GELATINBLOB_H
#define GELATINBLOB_H

#include "Wm5WindowApplication3.h"
#include "PhysicsModule.h"
using namespace Wm5;

class GelatinBlob : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    GelatinBlob ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void CreateScene ();
    void CreateIcosahedron ();
    void CreateSprings ();
    void CreateSegments ();
    void PhysicsTick ();
    void GraphicsTick ();

    // The scene graph.
    NodePtr mScene, mTrnNode, mSegments;
    WireStatePtr mWireState;
    TriMeshPtr mIcosahedron;
    Culler mCuller;

    // The physics system.
    PhysicsModule* mModule;

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(GelatinBlob);
WM5_REGISTER_TERMINATE(GelatinBlob);

#endif
