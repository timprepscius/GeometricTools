// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef FLUIDS2D_H
#define FLUIDS2D_H

#include "Wm5WindowApplication2.h"
#include "Smoke2D.h"
using namespace Wm5;

class Fluids2D : public WindowApplication2
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    Fluids2D ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnDisplay ();
    virtual void ScreenOverlay ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    Smoke2D<float>* mSmoke;
    ColorRGB mColor[256];
    bool mSingleStep;
    bool mDrawColored;
    bool mDrawVortices;
    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(Fluids2D);
WM5_REGISTER_TERMINATE(Fluids2D);

#endif
