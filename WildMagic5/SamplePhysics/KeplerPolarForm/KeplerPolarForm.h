// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef KEPLERPOLARFORM_H
#define KEPLERPOLARFORM_H

#include "Wm5WindowApplication2.h"
#include "PhysicsModule.h"
using namespace Wm5;

class KeplerPolarForm : public WindowApplication2
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    KeplerPolarForm ();

    virtual bool OnInitialize ();
    virtual void OnDisplay ();

protected:
    PhysicsModule mModule;
    std::vector<Vector2d> mPositions;
    int mSize;
};

WM5_REGISTER_INITIALIZE(KeplerPolarForm);
WM5_REGISTER_TERMINATE(KeplerPolarForm);

#endif
