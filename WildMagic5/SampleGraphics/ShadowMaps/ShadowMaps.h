// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef SHADOWMAPS_H
#define SHADOWMAPS_H

//----------------------------------------------------------------------------
// Motivated by the article "Soft-Edged Shadows"
// http://www.gamedev.net/reference/articles/article2193.asp
//----------------------------------------------------------------------------

#include "Wm5WindowApplication3.h"
#include "SMBlurEffect.h"
#include "SMSceneEffect.h"
#include "SMShadowEffect.h"
#include "SMUnlitEffect.h"
using namespace Wm5;

class ShadowMaps : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    ShadowMaps ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();

protected:
    void CreateScene ();
    void CreateScreenSpaceObjects ();
    void CreateShaders ();

    // The scene objects.
    NodePtr mScene;
    Culler mCuller;

    // Screen-space objects.
    CameraPtr mScreenCamera;
    TriMeshPtr mScreenPolygon;
    int mScreenTargetSize;
    RenderTargetPtr mShadowTarget;
    RenderTargetPtr mUnlitTarget;
    RenderTargetPtr mHBlurTarget;
    RenderTargetPtr mVBlurTarget;
    Float4 mShadowClear, mUnlitClear;

    // The effects.
    VisualEffectInstancePtr mPlaneSceneInstance;
    VisualEffectInstancePtr mSphereSceneInstance;
    SMShadowEffectPtr mShadowEffect;
    SMUnlitEffectPtr mUnlitEffect;
    VisualEffectInstancePtr mHBlurInstance;
    VisualEffectInstancePtr mVBlurInstance;

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(ShadowMaps);
WM5_REGISTER_TERMINATE(ShadowMaps);

#endif
