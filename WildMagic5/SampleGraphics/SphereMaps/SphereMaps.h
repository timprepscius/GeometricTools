// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2013/07/14)

#ifndef SPHEREMAPS_H
#define SPHEREMAPS_H

#include "Wm5WindowApplication3.h"
using namespace Wm5;

class SphereMaps : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    SphereMaps ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();

protected:
    void CreateScene ();

    // Mesh normals are duplicated to texture coordinates to avoid the AMD
    // lighting problems due to use of pre-OpenGL2.x extensions.  This is
    // called after each mScene->Update(time), because the skin controllers
    // modify vertices and normals.
    void CopyNormalToTCoord1 (Object* object);

    NodePtr mScene, mTrnNode;
    Culler mCuller;

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(SphereMaps);
WM5_REGISTER_TERMINATE(SphereMaps);

#endif
