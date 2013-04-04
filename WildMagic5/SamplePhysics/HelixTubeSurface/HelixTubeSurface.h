// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef HELIXTUBESURFACE_H
#define HELIXTUBESURFACE_H

#include "Wm5WindowApplication3.h"
using namespace Wm5;

class HelixTubeSurface : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    HelixTubeSurface ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);
    virtual bool OnSpecialKeyDown (int key, int x, int y);

protected:
    void CreateScene ();
    MultipleCurve3f* CreateCurve ();
    static float Radial (float t);
    virtual bool MoveCamera ();

    NodePtr mScene;
    WireStatePtr mWireState;
    Culler mCuller;
    MultipleCurve3f* mCurve;
    float mMinCurveTime, mMaxCurveTime, mCurvePeriod;
    float mCurveTime, mDeltaTime;

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(HelixTubeSurface);
WM5_REGISTER_TERMINATE(HelixTubeSurface);

#endif
