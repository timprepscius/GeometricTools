// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef GPUFLUIDS2D_H
#define GPUFLUIDS2D_H

#include "Wm5WindowApplication3.h"
#include "Smoke2D.h"
using namespace Wm5;

class GpuFluids2D : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    GpuFluids2D ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnPreidle ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    Smoke2D* mSmoke;
    bool mDrawColored;
    bool mDrawVortices;
    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(GpuFluids2D);
WM5_REGISTER_TERMINATE(GpuFluids2D);

#endif
