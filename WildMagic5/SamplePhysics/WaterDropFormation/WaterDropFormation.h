// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WATERDROPFORMATION_H
#define WATERDROPFORMATION_H

#include "Wm5WindowApplication3.h"
using namespace Wm5;

class WaterDropFormation : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    WaterDropFormation ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void CreateScene ();
    void CreatePlane ();
    void CreateWall ();
    void CreateWaterRoot ();

    void Configuration0 ();  // water surface
    void Configuration1 ();  // split into water surface and water drop
    void DoPhysical1 ();
    void DoPhysical2 ();
    void DoPhysical3 ();
    void PhysicsTick ();
    void GraphicsTick ();

    // The scene graph.
    NodePtr mScene, mTrnNode, mWaterRoot;
    WireStatePtr mWireState;
    TriMeshPtr mPlane, mWall;
    RevolutionSurfacePtr mWaterSurface, mWaterDrop;
    Texture2DEffectPtr mWaterEffect;
    Texture2DPtr mWaterTexture;
    Culler mCuller;

    // Water sphere curves and simulation parameters.
    NURBSCurve2f* mSpline;
    NURBSCurve2f* mCircle;
    Vector2f* mCtrlPoints;
    Vector2f* mTargets;
    float mSimTime, mSimDelta, mLastSeconds;

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(WaterDropFormation);
WM5_REGISTER_TERMINATE(WaterDropFormation);

#endif
