// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2012/07/07)

#ifndef CONVEXHULL2D_H
#define CONVEXHULL2D_H

#include "Wm5WindowApplication2.h"
using namespace Wm5;

class ConvexHull2D : public WindowApplication2
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    ConvexHull2D ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnDisplay ();
    virtual void ScreenOverlay ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void GenerateHull0D ();
    void GenerateHull1D ();
    void GenerateHull2D ();
    void GenerateHullManyCollinear ();
    void RegenerateHull ();
    int UnitToScreen (float value);

    float mEpsilon;
    Query::Type mQueryType;
    ConvexHullf* mHull;
    int mNumVertices;
    Vector2f* mVertices;

    int mSize;
    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(ConvexHull2D);
WM5_REGISTER_TERMINATE(ConvexHull2D);

#endif
