// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef NURBSCURVEEXAMPLE_H
#define NURBSCURVEEXAMPLE_H

#include "Wm5WindowApplication2.h"
using namespace Wm5;

class NURBSCurveExample : public WindowApplication2
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    NURBSCurveExample ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnDisplay ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void DoSimulation1 ();
    void DoSimulation2 ();
    void InitialConfiguration ();
    void NextConfiguration ();

    NURBSCurve2f* mSpline;
    NURBSCurve2f* mCircle;
    Vector2f* mCtrlPoints;
    Vector2f* mTargets;
    int mSize;
    float mH, mD;
    float mSimTime, mSimDelta;

    bool mDrawControlPoints;
};

WM5_REGISTER_INITIALIZE(NURBSCurveExample);
WM5_REGISTER_TERMINATE(NURBSCurveExample);

#endif
