// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef BSPLINECURVEEXAMPLES_H
#define BSPLINECURVEEXAMPLES_H

#include "Wm5WindowApplication2.h"
#include "Wm5BSplineCurve2.h"
using namespace Wm5;

class BSplineCurveExamples : public WindowApplication2
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    BSplineCurveExamples ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnDisplay ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    BSplineCurve2f* mSpline;
    int mNumCtrlPoints, mDegree;
    Vector2f* mCtrlPoint;
    float* mKnot;
    float mLocCtrlMin[6], mLocCtrlMax[6];
    int mCurveType;
    bool mModified;

    int mSize;
    float mV0, mV1, mV2;
};

WM5_REGISTER_INITIALIZE(BSplineCurveExamples);
WM5_REGISTER_TERMINATE(BSplineCurveExamples);

#endif
