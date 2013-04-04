// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.2.0 (2010/07/31)

#ifndef GPUROOTFINDER_H
#define GPUROOTFINDER_H

#include "Wm5WindowApplication3.h"
#include "RootFinderEffect.h"
using namespace Wm5;

class GpuRootFinder : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    GpuRootFinder ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();

protected:
    // In this example, the function is f(x) = (x-1.1)*(x+2.2).  You may
    // replace it by another function, in which case you also need to
    // modify "MyFunction" in RootFinder.fx and recompile via Compile.bat.
    // The assembly code in RootFinder.ps_3_0.txt and RootFinder.arbfp1.txt
    // must be copied and stringified in RootFinderEffect::msPPrograms.
    static float MyFunction (float x);

    CameraPtr mScreenCamera;
    RenderTargetPtr mRenderTarget;
    TriMeshPtr mScreenPolygon;
    Texture2D* mResults;
    Float4 mTextColor;
    std::set<float> mRoots;
};

WM5_REGISTER_INITIALIZE(GpuRootFinder);
WM5_REGISTER_TERMINATE(GpuRootFinder);

#endif
