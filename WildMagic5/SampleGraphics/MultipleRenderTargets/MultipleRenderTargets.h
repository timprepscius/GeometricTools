// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef MULTIPLERENDERTARGETS_H
#define MULTIPLERENDERTARGETS_H

#include "Wm5WindowApplication3.h"
using namespace Wm5;

class MultipleRenderTargets : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    MultipleRenderTargets ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void CreateScene ();

    NodePtr mScene, mTrnNode;
    WireStatePtr mWireState;
    Culler mCuller;

    CameraPtr mScreenCamera;
    RenderTargetPtr mRenderTarget;
    TriMeshPtr mScreenPolygon0, mScreenPolygon1;
    Float4 mClearWhite;
    Float4 mClearGray;

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(MultipleRenderTargets);
WM5_REGISTER_TERMINATE(MultipleRenderTargets);

#endif
