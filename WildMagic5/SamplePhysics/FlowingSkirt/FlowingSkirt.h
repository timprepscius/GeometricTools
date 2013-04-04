// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef FLOWINGSKIRT_H
#define FLOWINGSKIRT_H

#include "Wm5WindowApplication3.h"
#include "Wm5BSplineCurve3.h"
using namespace Wm5;

class FlowingSkirt : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    FlowingSkirt ();
    virtual ~FlowingSkirt ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void CreateScene ();
    void UpdateSkirt ();
    void ModifyCurves ();

    // The scene graph.
    NodePtr mScene, mTrnNode;
    TriMeshPtr mSkirt;
    WireStatePtr mWireState;
    Culler mCuller;

    // The skirt is a generalized Bezier cylinder.
    int mNumCtrl, mDegree;
    float mATop, mBTop, mABot, mBBot;
    BSplineCurve3f* mSkirtTop;
    BSplineCurve3f* mSkirtBot;
    float* mFrequencies;

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(FlowingSkirt);
WM5_REGISTER_TERMINATE(FlowingSkirt);

#endif
