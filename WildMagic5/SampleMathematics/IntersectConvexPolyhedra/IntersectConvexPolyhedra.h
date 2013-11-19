// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef INTERSECTCONVEXPOLYHEDRA_H
#define INTERSECTCONVEXPOLYHEDRA_H

#include "Wm5WindowApplication3.h"
#include "ConvexPolyhedron.h"
using namespace Wm5;

class IntersectConvexPolyhedra : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    IntersectConvexPolyhedra ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void CreateScene ();
    void ComputeIntersection ();

    NodePtr mScene;
    TriMeshPtr mMeshPoly0, mMeshPoly1, mMeshIntersection;
    Culler mCuller;

    ConvexPolyhedronf mWorldPoly0, mWorldPoly1, mIntersection;

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(IntersectConvexPolyhedra);
WM5_REGISTER_TERMINATE(IntersectConvexPolyhedra);

#endif
