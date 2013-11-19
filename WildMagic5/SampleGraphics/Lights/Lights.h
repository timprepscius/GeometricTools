// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef LIGHTS_H
#define LIGHTS_H

#include "Wm5WindowApplication3.h"
using namespace Wm5;

class Lights : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    Lights ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void CreateScene ();
    void CreatePlane ();
    void CreateSphere ();

    NodePtr mScene;
    WireStatePtr mWireState;
    TriMeshPtr mPlane0, mSphere0, mPlane1, mSphere1;
    Culler mCuller;

    // The available lights.  There are 3 light types (0 = directional,
    // 1 = point, 2 = spot) and 2 plane-sphere pairs (0 = per vertex,
    // 1 = per pixel).  Each light is shared by two effects.
    LightPtr mLight[3][2];
    std::string mCaption[3], mCurrentCaption;
    int mActiveType;

    // mInstance[i][j][k]:
    // i = light (0 = directional, 1 = point, 2 = spot)
    // j = effect (0 = per vertex, 1 = per pixel)
    // k = object (0 = plane, 1 = sphere)
    VisualEffectInstancePtr mInstance[3][2][2];

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(Lights);
WM5_REGISTER_TERMINATE(Lights);

#endif
