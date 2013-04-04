// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef SURFACEMESHES_H
#define SURFACEMESHES_H

#include "Wm5WindowApplication3.h"
#include "SimpleSegment.h"
#include "SimplePatch.h"
using namespace Wm5;

class SurfaceMeshes : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    SurfaceMeshes ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void CreateScene ();
    void CreateSimpleSegment ();
    void CreateSimplePatch ();

    NodePtr mScene;
    WireStatePtr mWireState;
    CullStatePtr mCullState;
    Culler mCuller;

    SimpleSegmentPtr mSegment;
    CurveMeshPtr mCurve;
    SimplePatchPtr mPatch;
    SurfaceMeshPtr mSurface;
    int mLevel;

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(SurfaceMeshes);
WM5_REGISTER_TERMINATE(SurfaceMeshes);

#endif
