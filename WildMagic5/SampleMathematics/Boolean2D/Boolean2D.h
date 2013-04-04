// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef BOOLEAN2D_H
#define BOOLEAN2D_H

#include "Wm5WindowApplication2.h"
#include "BspPolygon2.h"
using namespace Wm5;

class Boolean2D : public WindowApplication2
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    Boolean2D ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnDisplay ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    BspPolygon2* ConstructInvertedEll ();
    BspPolygon2* ConstructPentagon ();
    BspPolygon2* ConstructSquare ();
    BspPolygon2* ConstructSShape ();
    BspPolygon2* ConstructPolyWithHoles ();

    void DoBoolean ();
    void DrawPolySolid (BspPolygon2& polygon, ColorRGB color);

    BspPolygon2 mIntersection, mUnion, mDiff01, mDiff10, mXor;
    BspPolygon2* mPoly0;
    BspPolygon2* mPoly1;
    BspPolygon2* mActive;
    int mChoice;
    int mSize;
};

WM5_REGISTER_INITIALIZE(Boolean2D);
WM5_REGISTER_TERMINATE(Boolean2D);

#endif
