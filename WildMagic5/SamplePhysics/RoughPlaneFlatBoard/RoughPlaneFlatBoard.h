// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef ROUGHPLANEFLATBOARD_H
#define ROUGHPLANEFLATBOARD_H

#include "Wm5WindowApplication2.h"
#include "PhysicsModule.h"
using namespace Wm5;

class RoughPlaneFlatBoard : public WindowApplication2
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    RoughPlaneFlatBoard ();

    virtual bool OnInitialize ();
    virtual void OnIdle ();
    virtual void OnDisplay ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    PhysicsModule mModule;
    int mSize;
};

WM5_REGISTER_INITIALIZE(RoughPlaneFlatBoard);
WM5_REGISTER_TERMINATE(RoughPlaneFlatBoard);

#endif
