// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef CLIPMESH_H
#define CLIPMESH_H

#include "Wm5WindowApplication3.h"
using namespace Wm5;

class ClipMesh : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    ClipMesh ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void CreateScene ();
    void Update ();

    // A torus.
    std::vector<APoint> mTorusVerticesMS, mTorusVerticesWS;
    std::vector<int> mTorusIndices;
    HPlane mPlane;

    NodePtr mScene;
    TriMeshPtr mTorus, mMeshPlane;
    WireStatePtr mTorusWireState;
    Culler mCuller;

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(ClipMesh);
WM5_REGISTER_TERMINATE(ClipMesh);

#endif
