// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef CONFORMALMAPPING_H
#define CONFORMALMAPPING_H

#include "Wm5WindowApplication3.h"
using namespace Wm5;

class ConformalMapping : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    ConformalMapping ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void CreateScene ();
    void ScaleToCube (float extreme, TriMesh* mesh);
    void PseudocolorVertices (TriMesh* mesh);
    TriMesh* DoMapping (TriMesh* mesh);

    NodePtr mScene, mMeshTree, mSphereTree;
    WireStatePtr mWireState;
    Culler mCuller;
    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(ConformalMapping);
WM5_REGISTER_TERMINATE(ConformalMapping);

#endif
