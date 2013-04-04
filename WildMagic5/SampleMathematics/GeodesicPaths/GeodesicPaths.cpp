// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "GeodesicPaths.h"
using namespace Wm5;

WM5_WINDOW_APPLICATION(GeodesicPaths);

//----------------------------------------------------------------------------
GeodesicPaths::GeodesicPaths ()
    :
    WindowApplication2("SampleMathematics/GeodesicPaths", 0, 0, 512, 512,
        Float4(1.0f, 1.0f, 1.0f, 1.0f)),
    mGeodesic(1.0f, 1.0f, 1.0f),
    mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
    mGeodesic.RefineCallback = &GeodesicPaths::RefineCallback;

    mNumTruePoints = 129;
    mTruePoints = new1<GVectorf>(mNumTruePoints);
    int i;
    for (i = 0; i < mNumTruePoints; ++i)
    {
        mTruePoints[i].SetSize(2);
    }

    mNumApprPoints = (1 << mGeodesic.Subdivisions) + 1;
    mApprPoints = new1<GVectorf>(mNumApprPoints);
    for (i = 0; i < mNumApprPoints; ++i)
    {
        mApprPoints[i].SetSize(2);
    }

    mParam0.SetSize(2);
    mParam1.SetSize(2);

    mSize = GetWidth();
}
//----------------------------------------------------------------------------
GeodesicPaths::~GeodesicPaths ()
{
    delete1(mApprPoints);
    delete1(mTruePoints);
}
//----------------------------------------------------------------------------
bool GeodesicPaths::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    // Only process the first octant of the ellipsoid.
    mXMin = 0.0f;
    mXMax = Mathf::HALF_PI;
    mXDelta = (mXMax - mXMin)/(float)mSize;
    mYMin = Mathf::HALF_PI/(float)mSize;
    mYMax = Mathf::HALF_PI;
    mYDelta = (mYMax - mYMin)/(float)mSize;

    ComputeTruePath();

    DoFlip(true);
    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void GeodesicPaths::OnTerminate ()
{
    WindowApplication2::OnTerminate();
}
//----------------------------------------------------------------------------
void GeodesicPaths::OnDisplay ()
{
    ClearScreen();

    ColorRGB red(255, 0, 0), green(0, 255, 0);
    int i, x0, y0, x1, y1;

    // Draw the true path.
    ParamToXY(mTruePoints[0], x0, y0);
    for (i = 1; i < mNumTruePoints; ++i)
    {
        ParamToXY(mTruePoints[i], x1, y1);
        DrawLine(x0, y0, x1, y1, green);
        x0 = x1;
        y0 = y1;
    }

    // Draw the approximate path.
    int numApprPoints = mGeodesic.GetCurrentQuantity();
    if (numApprPoints == 0)
    {
        numApprPoints = mCurrNumApprPoints;
    }

    ParamToXY(mApprPoints[0], x0, y0);
    for (i = 1; i < numApprPoints; ++i)
    {
        ParamToXY(mApprPoints[i], x1, y1);
        DrawLine(x0, y0, x1, y1, red);
        x0 = x1;
        y0 = y1;
    }

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
void GeodesicPaths::ScreenOverlay ()
{
    char message[512];
    sprintf(message, "true dist = %f, appr dist = %f, appr curv = %f",
        mTrueDistance, mApprDistance, mApprCurvature);

    mRenderer->Draw(8, 16, mTextColor, message);

    sprintf(message, "sub = %d, ref = %d, currquan = %d",
        mGeodesic.GetSubdivisionStep(), mGeodesic.GetRefinementStep(),
        mGeodesic.GetCurrentQuantity());

    mRenderer->Draw(8, 32, mTextColor, message);
}
//----------------------------------------------------------------------------
bool GeodesicPaths::OnKeyDown (unsigned char key, int x, int y)
{
    if (WindowApplication2::OnKeyDown(key, x, y))
    {
        return true;
    }

    switch (key)
    {
    case '0':
        ComputeTruePath();
        OnDisplay();
        return true;
    case '1':
        ComputeApprPath(true);
        OnDisplay();
        return true;
    case '2':
        ComputeApprPath(false);
        OnDisplay();
        return true;
    case '3':
        delete1(mApprPoints);
        mApprPoints = 0;

        mGeodesic.ComputeGeodesic(mParam0, mParam1, mCurrNumApprPoints,
            mApprPoints);

        ComputeApprLength();
        OnDisplay();
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void GeodesicPaths::ComputeTruePath ()
{
    // Random selection of endpoints.  The angles are (theta,phi) with
    // 0 <= theta < 2*pi and 0 <= phi < pi/2, thus placing the points on the
    // the first octant of the ellipsoid.
    mParam0[0] = Mathf::IntervalRandom(0.0f, Mathf::HALF_PI);
    mParam0[1] = Mathf::IntervalRandom(0.0f, Mathf::HALF_PI);
    mParam1[0] = Mathf::IntervalRandom(0.0f, Mathf::HALF_PI);
    mParam1[1] = Mathf::IntervalRandom(0.0f, Mathf::HALF_PI);

    // Compute the true geodesic path.
    Vector3f pos0 = mGeodesic.ComputePosition(mParam0);
    Vector3f pos1 = mGeodesic.ComputePosition(mParam1);
    float angle = Mathf::ACos(pos0.Dot(pos1));
    for (int i = 0; i < mNumTruePoints; ++i)
    {
        float t = i/(float)(mNumTruePoints - 1);
        float sn0 = Mathf::Sin((1.0f - t)*angle);
        float sn1 = Mathf::Sin(t*angle);
        float sn = Mathf::Sin(angle);
        Vector3f pos = (sn0*pos0 + sn1*pos1)/sn;
        mTruePoints[i][0] = Mathf::ATan2(pos.Y(), pos.X());
        mTruePoints[i][1] = Mathf::ACos(pos.Z());
    }

    // Compute the true length of the geodesic path.
    mTrueDistance = angle;

    // Initialize the approximate path.
    mCurrNumApprPoints = 2;
    mApprPoints[0] = mParam0;
    mApprPoints[1] = mParam1;
    ComputeApprLength();
}
//----------------------------------------------------------------------------
void GeodesicPaths::ComputeApprPath (bool subdivide)
{
    int i;

    if (subdivide)
    {
        int newNumApprPoints = 2*mCurrNumApprPoints - 1;
        if (newNumApprPoints > mNumApprPoints)
        {
            return;
        }

        // Copy the old points so that there are slots for the midpoints
        // during the subdivision interleaved between the old points.
        for (i = mCurrNumApprPoints - 1; i > 0; --i)
        {
            mApprPoints[2*i] = mApprPoints[i];
        }

        for (i = 0; i <= mCurrNumApprPoints - 2; ++i)
        {
            mGeodesic.Subdivide(mApprPoints[2*i], mApprPoints[2*i + 1],
                mApprPoints[2*i + 2]);
        }

        mCurrNumApprPoints = newNumApprPoints;
    }
    else // refine
    {
        for (i = 1; i <= mCurrNumApprPoints - 2; ++i)
        {
            mGeodesic.Refine(mApprPoints[i - 1], mApprPoints[i],
                mApprPoints[i + 1]);
        }
    }

    ComputeApprLength();
}
//----------------------------------------------------------------------------
void GeodesicPaths::ComputeApprLength ()
{
    int numApprPoints = mGeodesic.GetCurrentQuantity();
    if (numApprPoints == 0)
    {
        numApprPoints = mCurrNumApprPoints;
    }

    mApprDistance = mGeodesic.ComputeTotalLength(numApprPoints, mApprPoints);
    mApprCurvature = mGeodesic.ComputeTotalCurvature(numApprPoints,
        mApprPoints);
}
//----------------------------------------------------------------------------
void GeodesicPaths::ParamToXY (const GVectorf& param, int& x, int& y)
{
    // Only the first octant of the ellipsoid is used.
    x = (int)((param[0] - mXMin)/mXDelta + 0.5f);
    y = (int)((param[1] - mYMin)/mYDelta + 0.5f);
}
//----------------------------------------------------------------------------
void GeodesicPaths::XYToParam (int x, int y, GVectorf& param)
{
    param[0] = mXMin + x*mXDelta;
    param[1] = mYMin + y*mYDelta;
}
//----------------------------------------------------------------------------
void GeodesicPaths::RefineCallback ()
{
    GeodesicPaths* app = (GeodesicPaths*)TheApplication;
    app->ComputeApprLength();
    app->OnDisplay();
}
//----------------------------------------------------------------------------
