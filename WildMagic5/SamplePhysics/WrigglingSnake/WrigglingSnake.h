// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WRIGGLINGSNAKE_H
#define WRIGGLINGSNAKE_H

#include "Wm5WindowApplication3.h"
using namespace Wm5;

class WrigglingSnake : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    WrigglingSnake ();
    virtual ~WrigglingSnake ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void CreateScene ();
    void CreateSnake ();
    void CreateSnakeBody ();
    void CreateSnakeHead ();
    void UpdateSnake ();
    void ModifyCurve ();
    static float Radial (float t);

    // The scene graph.
    NodePtr mScene, mTrnNode, mSnakeRoot;
    TubeSurfacePtr mSnakeBody;
    TriMeshPtr mSnakeHead;
    WireStatePtr mWireState;
    Culler mCuller;

    // The curve and parameters for the snake body.
    int mNumCtrlPoints, mDegree;
    BSplineCurve3f* mCenter;
    float* mAmplitudes;
    float* mPhases;
    int mNumShells;
    Vector3f* mSlice;
    static float msRadius;

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(WrigglingSnake);
WM5_REGISTER_TERMINATE(WrigglingSnake);

#endif
