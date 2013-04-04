// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef GELATINCUBE_H
#define GELATINCUBE_H

#include "Wm5WindowApplication3.h"
#include "PhysicsModule.h"
using namespace Wm5;

class GelatinCube : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    GelatinCube ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void CreateScene ();
    void CreateSprings ();
    void CreateBox ();
    void PhysicsTick ();
    void GraphicsTick ();

    // The scene graph
    NodePtr mScene, mTrnNode;
    WireStatePtr mWireState;
    BoxSurfacePtr mBox;
    Culler mCuller;

    // The masses are located at the control points of a spline surface.
    // The control points are connected in a mass-spring system.
    BSplineVolumef* mSpline;
    PhysicsModule* mModule;

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(GelatinCube);
WM5_REGISTER_TERMINATE(GelatinCube);

#endif
