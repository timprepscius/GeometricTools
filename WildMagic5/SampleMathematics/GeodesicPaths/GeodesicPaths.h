// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef GEODESICPATHS_H
#define GEODESICPATHS_H

#include "Wm5WindowApplication2.h"
using namespace Wm5;

class GeodesicPaths : public WindowApplication2
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    GeodesicPaths ();
    virtual ~GeodesicPaths ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual void OnDisplay ();
    virtual void ScreenOverlay ();
    virtual bool OnKeyDown (unsigned char key, int x, int y);

protected:
    void ComputeTruePath ();
    void ComputeApprPath (bool subdivide);
    void ComputeApprLength ();
    void ParamToXY (const GVectorf& param, int& x, int& y);
    void XYToParam (int x, int y, GVectorf& param);
    static void RefineCallback ();

    EllipsoidGeodesicf mGeodesic;
    GVectorf mParam0, mParam1;
    float mXMin, mXMax, mXDelta;
    float mYMin, mYMax, mYDelta;

    int mNumTruePoints;
    GVectorf* mTruePoints;
    float mTrueDistance;

    int mNumApprPoints;
    int mCurrNumApprPoints;
    GVectorf* mApprPoints;
    float mApprDistance;
    float mApprCurvature;

    int mSize;
    Float4 mTextColor;
};

WM5_REGISTER_INITIALIZE(GeodesicPaths);
WM5_REGISTER_TERMINATE(GeodesicPaths);

#endif
