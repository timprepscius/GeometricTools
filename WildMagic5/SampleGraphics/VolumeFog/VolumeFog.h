// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef VOLUMEFOG_H
#define VOLUMEFOG_H

#include "Wm5WindowApplication3.h"
using namespace Wm5;

class VolumeFog : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    VolumeFog ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();

protected:
    void CreateScene ();
    void UpdateFog ();

    NodePtr mScene;
    TriMeshPtr mMesh;
    CameraPtr mScreenCamera;
    TriMeshPtr mScreenPolygon;
    Culler mCuller;

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(VolumeFog);
WM5_REGISTER_TERMINATE(VolumeFog);

#endif
