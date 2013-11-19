// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.7.0 (2011/08/29)

#ifndef INTERSECTINFINITECYLINDERS_H
#define INTERSECTINFINITECYLINDERS_H

#include "Wm5WindowApplication3.h"
using namespace Wm5;

class IntersectInfiniteCylinders : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    IntersectInfiniteCylinders ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void CreateScene ();

    Float4 mTextColor;
    NodePtr mScene;
    WireStatePtr mWireState;
    TriMeshPtr mCylinder0, mCylinder1;
    float mRadius0, mRadius1, mHeight, mAngle;
    float mC0, mW1, mW2;
    PolysegmentPtr mCurve0, mCurve1;
    VisibleSet mVisible;
};

WM5_REGISTER_INITIALIZE(IntersectInfiniteCylinders);
WM5_REGISTER_TERMINATE(IntersectInfiniteCylinders);

#endif
