// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef TRISTRIPSFANS_H
#define TRISTRIPSFANS_H

#include "Wm5WindowApplication3.h"
using namespace Wm5;

class TriStripsFans : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    TriStripsFans ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void CreateScene ();

    NodePtr mScene;
    WireStatePtr mWireState;
    TriStripPtr mStrip;
    TriMeshPtr mStripMesh;
    TriFanPtr mFan;
    TriMeshPtr mFanMesh;
    Triangles* mActive;
    Culler mCuller;

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(TriStripsFans);
WM5_REGISTER_TERMINATE(TriStripsFans);

#endif
