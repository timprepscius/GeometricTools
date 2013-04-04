// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef CLOTH_H
#define CLOTH_H

#include "Wm5WindowApplication3.h"
#include "PhysicsModule.h"
using namespace Wm5;


class Cloth : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    Cloth ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void CreateSprings ();
    void CreateCloth ();
    void CreateScene ();
    void PhysicsTick ();
    void GraphicsTick ();

    // Masses are located at the control points of a spline surface.
    BSplineRectanglef* mSpline;

    // The mass-spring physics system.
    PhysicsModule* mModule;

    // The scene graph.
    NodePtr mScene, mTrnNode;
    WireStatePtr mWireState;
    RectangleSurfacePtr mCloth;
    Culler mCuller;

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(Cloth);
WM5_REGISTER_TERMINATE(Cloth);

#endif
