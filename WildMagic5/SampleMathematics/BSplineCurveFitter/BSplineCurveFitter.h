// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef BSPLINECURVEFITTER_H
#define BSPLINECURVEFITTER_H

#include "Wm5WindowApplication3.h"
using namespace Wm5;

class BSplineCurveFitter : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    BSplineCurveFitter ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void CreateScene ();
    void CreateBSplinePolysegment ();

    NodePtr mScene;
    VertexColor3EffectPtr mEffect;
    Culler mCuller;

    int mNumSamples;
    Vector3f* mSamples;
    int mDegree;
    int mNumCtrlPoints;
    BSplineCurveFitf* mSpline;

    float mAvrError, mRmsError;
    char mMessage[128];
    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(BSplineCurveFitter);
WM5_REGISTER_TERMINATE(BSplineCurveFitter);

#endif
