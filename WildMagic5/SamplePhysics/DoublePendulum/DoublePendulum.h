// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef DOUBLEPENDULUM_H
#define DOUBLEPENDULUM_H

#include "Wm5WindowApplication2.h"
#include "PhysicsModule.h"
using namespace Wm5;

class DoublePendulum : public WindowApplication2
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    DoublePendulum ();

    virtual bool OnInitialize ();
    virtual void OnIdle ();
    virtual void OnDisplay ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    PhysicsModule mModule;
    int mSize;
};

WM5_REGISTER_INITIALIZE(DoublePendulum);
WM5_REGISTER_TERMINATE(DoublePendulum);

#endif
