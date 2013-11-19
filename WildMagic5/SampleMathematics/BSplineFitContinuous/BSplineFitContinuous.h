// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef BSPLINEFITCONTINUOUS_H
#define BSPLINEFITCONTINUOUS_H

#include "Wm5WindowApplication3.h"
using namespace Wm5;

class BSplineFitContinuous : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    BSplineFitContinuous ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();

protected:
    void CreateScene ();

    Polysegment* OriginalPolysegment (int numCtrlPoints,
        Vector3d* ctrlPoints);

    Polysegment* ReducedPolysegment (int numCtrlPoints, Vector3d* ctrlPoints,
        double fraction);

    NodePtr mScene, mTrnNode;
    Culler mCuller;
    int mDegree;

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(BSplineFitContinuous);
WM5_REGISTER_TERMINATE(BSplineFitContinuous);

#endif
