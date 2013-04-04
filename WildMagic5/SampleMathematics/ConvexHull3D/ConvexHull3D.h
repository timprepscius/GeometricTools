// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2012/07/07)

#ifndef CONVEXHULL3D_H
#define CONVEXHULL3D_H

#include "Wm5WindowApplication3.h"
using namespace Wm5;

class ConvexHull3D : public WindowApplication3
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    ConvexHull3D ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnIdle ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);
    virtual bool OnMouseClick (int button, int state, int x, int y,
        unsigned int modifiers);

protected:
    void CreateScene ();
    void LoadData ();
    void CreateHull ();
    void RegenerateHull ();
    TriMesh* CreateSphere ();

    // The input data files are in the Data subfolder.  The files are of the
    // format "dataXX.txt", where XX is in {01,02,...,46}.
    int mFileQuantity;  // = 46
    int mCurrentFile;  // = 1 initially

    // The input data sets with randomly generated vertex colors.
    int mNumVertices;
    Vector3f* mVertices;
    Float3* mColors;

    // The query type for the hull construction and the hull itself.
    Query::Type mQueryType;
    ConvexHullf* mHull;

    // A visual representation of the hull.
    NodePtr mScene, mTrnNode, mHullNode, mSphere;
    WireStatePtr mWireState;
    CullStatePtr mCullState;
    Culler mCuller;

    // For picking.
    Picker mPicker;

    // For debugging purposes.  The line of code
    //    mLimitedQuantity = mNumVertices;
    // in LoadData(...) can be replaced by
    //    mLimitedQuantity = 3;
    // This allows a subset of the input data set to be used for hull
    // construction.  The '+' key increments mLimitedQuantity and causes
    // the hull to be computed; the '-' key decrements mLimitedQuantity and
    // causes the hull to be computed.  This is useful for query type REAL
    // when the hull appears to be incorrect.  You can increment
    // mLimitedQuantity while the hull appears to be correct, and trap the
    // case when it fails.  Subsequently, you can set a conditional breakpoint
    // to step into the hull construction to see where things are failing.
    // Assuming the hull construction algorithm is correct, the failure is a
    // result of numerical round-off error leading to topological problems
    // with the convex polyhedron mesh.
    int mLimitedQuantity;

    // Display of query type and of hull vertex/triangle information.
    char mHeader[256], mFooter[256];
    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(ConvexHull3D);
WM5_REGISTER_TERMINATE(ConvexHull3D);

#endif
