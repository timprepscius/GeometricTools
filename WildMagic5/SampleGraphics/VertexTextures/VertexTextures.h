// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 4.10.0 (2009/11/18)

#ifndef VERTEXTEXTURES_H
#define VERTEXTEXTURES_H

#include "Wm5WindowApplication3.h"
using namespace Wm5;

// DX9 profile vs_3_0 and OpenGL profile vp40 support vertex textures.  You
// cannot run this sample without hardware supporting these profiles.

class VertexTextures : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    VertexTextures ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();

protected:
    void CreateScene ();

    NodePtr mScene;
    Culler mCuller;

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(VertexTextures);
WM5_REGISTER_TERMINATE(VertexTextures);

#endif
