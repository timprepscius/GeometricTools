// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef EXTREMALQUERY_H
#define EXTREMALQUERY_H

#include "Wm5WindowApplication3.h"
#include "Wm5ExtremalQuery3.h"
using namespace Wm5;

// Expose this for the BSP-based query.  Comment it out for the
// projection-based query.
//#define USE_BSP_QUERY

// Uncomment this for timing information.
//#define MEASURE_TIMING_OF_QUERY

class ExtremalQuery : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    ExtremalQuery ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);
    virtual bool OnMotion (int button, int x, int y, unsigned int modifiers);

protected:
    void CreateScene ();
    void CreateConvexPolyhedron (int numVertices);
    Node* CreateVisualConvexPolyhedron ();
    void UpdateExtremePoints ();

    ConvexPolyhedron3f* mConvexPolyhedron;
    ExtremalQuery3f* mExtremalQuery;

    NodePtr mScene;
    WireStatePtr mWireState;
    CullStatePtr mCullState;
    TriMeshPtr mMaxSphere, mMinSphere;
    Culler mCuller;

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(ExtremalQuery);
WM5_REGISTER_TERMINATE(ExtremalQuery);

#endif
