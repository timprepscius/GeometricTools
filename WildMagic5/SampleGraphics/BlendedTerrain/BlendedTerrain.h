// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef BLENDEDTERRAIN_H
#define BLENDEDTERRAIN_H

#include "Wm5WindowApplication3.h"
#include "BlendedTerrainEffect.h"
using namespace Wm5;

class BlendedTerrain : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    BlendedTerrain ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void CreateScene ();
    void UpdateClouds ();

    NodePtr mScene;
    WireStatePtr mWireState;
    TriMeshPtr mSkyDome;
    Culler mCuller;

    float* mFlowDirection;
    float* mPowerFactor;
    float mFlowDelta, mZAngle, mZDeltaAngle;

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(BlendedTerrain);
WM5_REGISTER_TERMINATE(BlendedTerrain);

#endif
