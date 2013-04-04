// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef POLYHEDRONDISTANCE_H
#define POLYHEDRONDISTANCE_H

#include "Wm5WindowApplication3.h"
using namespace Wm5;

class PolyhedronDistance : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    PolyhedronDistance ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void CreateScene ();
    TriMesh* CreateTetra (float size, bool isBlack);
    Polysegment* CreateSegment ();
    TriMesh* CreatePlane ();
    void UpdateSegments ();
    bool ApplyTransform (unsigned char key);

    // Representation of bodies.
    Tuple<3,int>* mFaces;
    TriMesh* mTetras[4];

    // Display variables.
    float mSeparation;
    float mEdgeLength;
    float mSmall;

    // Offsets during calculation with LCPPolyDist to ensure that all of the
    // vertices are in the first octant.
    float mOffsetMagnitude;
 
    // The scene graph.
    NodePtr mScene;
    WireStatePtr mWireState;
    PolysegmentPtr mSegments[2];
    Culler mCuller;

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(PolyhedronDistance);
WM5_REGISTER_TERMINATE(PolyhedronDistance);

#endif
