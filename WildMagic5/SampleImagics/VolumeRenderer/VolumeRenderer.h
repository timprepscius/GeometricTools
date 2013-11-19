// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef VOLUMERENDERER_H
#define VOLUMERENDERER_H

#include "Wm5WindowApplication2.h"
#include "RayTrace.h"
using namespace Wm5;

class VolumeRenderer : public WindowApplication2
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    VolumeRenderer ();

    virtual bool OnPrecreate ();
    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnDisplay ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);
    virtual bool OnMouseClick (int button, int state, int x, int y,
        unsigned int modifiers);
    virtual bool OnMotion (int button, int x, int y, unsigned int modifiers);

protected:
    int mBound;
    RayTrace* mRT;
    float mX0, mY0, mX1, mY1, mHBound, mGamma;
    bool mButtonDown;
};

WM5_REGISTER_INITIALIZE(VolumeRenderer);
WM5_REGISTER_TERMINATE(VolumeRenderer);

#endif
