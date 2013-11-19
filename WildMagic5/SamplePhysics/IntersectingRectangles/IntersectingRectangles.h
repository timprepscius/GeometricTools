// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef INTERSECTINGRECTANGLES_H
#define INTERSECTINGRECTANGLES_H

#include "Wm5WindowApplication2.h"
using namespace Wm5;

class IntersectingRectangles : public WindowApplication2
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    IntersectingRectangles ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual void OnDisplay ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void ModifyRectangles ();
    void DrawRectangles ();

    std::vector<AxisAlignedBox2f> mRectangles;
    RectangleManagerf* mManager;
    float mLastIdle;
    int mSize;
    bool mMouseDown;
};

WM5_REGISTER_INITIALIZE(IntersectingRectangles);
WM5_REGISTER_TERMINATE(IntersectingRectangles);

#endif
