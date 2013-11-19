// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef POINTINPOLYHEDRON_H
#define POINTINPOLYHEDRON_H

#include "Wm5WindowApplication3.h"
using namespace Wm5;

class PointInPolyhedron : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    PointInPolyhedron ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void CreateScene ();
    void CreateQuery (TriMesh* mesh);
    void DeleteQuery ();

    NodePtr mScene;
    WireStatePtr mWireState;
    PolypointPtr mPoints;
    Culler mCuller;

    PointInPolyhedron3f* mQuery;
    PointInPolyhedron3f::TriangleFace* mTFaces;
    PointInPolyhedron3f::ConvexFace* mCFaces;
    PointInPolyhedron3f::SimpleFace* mSFaces;
    int mNumRays;
    Vector3f* mRayDirections;

    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(PointInPolyhedron);
WM5_REGISTER_TERMINATE(PointInPolyhedron);

#endif
