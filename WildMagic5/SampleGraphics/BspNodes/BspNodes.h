// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef BSPNODES_H
#define BSPNODES_H

#include "Wm5WindowApplication3.h"
using namespace Wm5;

class BspNodes : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    BspNodes ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();

protected:
    void CreateScene ();
    BspNode* CreateNode (const Vector2f& v0, const Vector2f& v1,
        VertexColor3Effect* effect, const Float3& color);

    NodePtr mScene;
    Culler mCuller;

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(BspNodes);
WM5_REGISTER_TERMINATE(BspNodes);

#endif
