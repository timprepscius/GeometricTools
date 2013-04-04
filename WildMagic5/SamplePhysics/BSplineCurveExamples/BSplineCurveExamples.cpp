// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "BSplineCurveExamples.h"

WM5_WINDOW_APPLICATION(BSplineCurveExamples);

//----------------------------------------------------------------------------
BSplineCurveExamples::BSplineCurveExamples ()
    :
    WindowApplication2("SamplePhysics/BSplineCurveExamples", 0, 0, 128, 128,
        Float4(1.0f, 1.0f, 1.0f, 1.0f))
{
    mSpline = 0;
    mCtrlPoint = 0;
    mKnot = 0;
    mCurveType = 0;
    mModified = false;

    mLocCtrlMin[0] = 1.0f/3.0f;
    mLocCtrlMax[0] = 5.0f/6.0f;
    mLocCtrlMin[1] = 1.0f/5.0f;
    mLocCtrlMax[1] = 4.0f/5.0f;
    mLocCtrlMin[2] = 1.0f/3.0f;
    mLocCtrlMax[2] = 5.0f/6.0f;
    mLocCtrlMin[3] = 2.0f/7.0f;
    mLocCtrlMax[3] = 5.0f/7.0f;
    mLocCtrlMin[4] = 1.0f/5.0f;
    mLocCtrlMax[4] = 4.0f/5.0f;
    mLocCtrlMin[5] = 1.0f/4.0f;
    mLocCtrlMax[5] = 5.0f/8.0f;

    mSize = GetWidth();
    mV0 = mSize/16.0f;
    mV1 = 0.5f*mSize;
    mV2 = 15.0f*mSize/16.0f;
}
//----------------------------------------------------------------------------
bool BSplineCurveExamples::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    mNumCtrlPoints = 8;
    mDegree = 2;
    mCtrlPoint = new1<Vector2f>(mNumCtrlPoints);

    mCtrlPoint[0] = Vector2f(mV0, mV0);
    mCtrlPoint[1] = Vector2f(mV1, mV0);
    mCtrlPoint[2] = Vector2f(mV2, mV0);
    mCtrlPoint[3] = Vector2f(mV2, mV1);
    mCtrlPoint[4] = Vector2f(mV2, mV2);
    mCtrlPoint[5] = Vector2f(mV1, mV2);
    mCtrlPoint[6] = Vector2f(mV0, mV2);
    mCtrlPoint[7] = Vector2f(mV0, mV1);

    // open uniform, not closed
    mSpline = new0 BSplineCurve2f(mNumCtrlPoints, mCtrlPoint, mDegree,
        false, true);

    // Knots for later use.  The first 5 are used for the not-closed curve.
    // The first 6 are used for the closed curve.
    mKnot = new1<float>(6);
    mKnot[0] = 0.1f;
    mKnot[1] = 0.2f;
    mKnot[2] = 0.4f;
    mKnot[3] = 0.7f;
    mKnot[4] = 0.8f;
    mKnot[5] = 0.9f;

    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void BSplineCurveExamples::OnTerminate ()
{
    delete0(mSpline);
    delete1(mCtrlPoint);
    delete1(mKnot);
    WindowApplication2::OnTerminate();
}
//----------------------------------------------------------------------------
void BSplineCurveExamples::OnDisplay ()
{
    ClearScreen();

    ColorRGB lightGray(224, 224, 224);
    ColorRGB mediumGray(192, 192, 192);
    ColorRGB darkGray(128, 128, 128);
    ColorRGB black(0, 0, 0);

    // draw axes
    int i;
    for (i = mSize/16; i < mSize; ++i)
    {
        SetPixel(mSize/16, mSize - 1 - i, lightGray);
        SetPixel(i, mSize - 1 - mSize/16, lightGray);
    }

    // draw control points
    int imax = mSpline->GetNumCtrlPoints();
    int x, y;
    for (i = 0; i < imax; ++i)
    {
        const Vector2f& ctrl = mSpline->GetControlPoint(i);
        x = (int)(ctrl.X() + 0.5f);
        y = mSize - 1 - (int)(ctrl.Y() + 0.5f);
        SetThickPixel(x, y, 2, darkGray);
    }

    // draw spline
    imax = 2048;
    for (i = 0; i <= imax; ++i)
    {
        // draw point
        float u = i/(float)imax;
        Vector2f pos = mSpline->GetPosition(u);
        x = (int)(pos.X() + 0.5f);
        y = mSize - 1 - (int)(pos.Y() + 0.5f);

        if (mModified
        &&  mLocCtrlMin[mCurveType] <= u && u <= mLocCtrlMax[mCurveType])
        {
            SetPixel(x, y, mediumGray);
        }
        else
        {
            SetPixel(x, y, black);
        }
    }

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
bool BSplineCurveExamples::OnKeyDown (unsigned char key, int x, int y)
{
    if (WindowApplication2::OnKeyDown(key, x, y))
    {
        return true;
    }

    switch (key)
    {
    case '0':  // open uniform, not-closed
        delete0(mSpline);
        mSpline = new0 BSplineCurve2f(mNumCtrlPoints, mCtrlPoint, mDegree,
            false, true);
        mModified = false;
        mCurveType = 0;
        OnDisplay();
        return true;
    case '1':  // open nonuniform, not-closed
        delete0(mSpline);
        mSpline = new0 BSplineCurve2f(mNumCtrlPoints, mCtrlPoint, mDegree,
            false, mKnot);
        mModified = false;
        mCurveType = 1;
        OnDisplay();
        return true;
    case '2':  // periodic, not-closed
        delete0(mSpline);
        mSpline = new0 BSplineCurve2f(mNumCtrlPoints, mCtrlPoint, mDegree,
            false, false);
        mModified = false;
        mCurveType = 2;
        OnDisplay();
        return true;
    case '3':  // open uniform, closed
        delete0(mSpline);
        mSpline = new0 BSplineCurve2f(mNumCtrlPoints, mCtrlPoint, mDegree,
            true, true);
        mModified = false;
        mCurveType = 3;
        OnDisplay();
        return true;
    case '4':  // open nonuniform, closed
        delete0(mSpline);
        mSpline = new0 BSplineCurve2f(mNumCtrlPoints, mCtrlPoint, mDegree,
            true, mKnot);
        mModified = false;
        mCurveType = 4;
        OnDisplay();
        return true;
    case '5':  // periodic, closed
        delete0(mSpline);
        mSpline = new0 BSplineCurve2f(mNumCtrlPoints, mCtrlPoint, mDegree,
            true, false);
        mModified = false;
        mCurveType = 5;
        OnDisplay();
        return true;
    case 'm':  // modify a control point
        mSpline->SetControlPoint(4, Vector2f(mSize - 2.0f, mSize - 2.0f));
        mModified = true;
        OnDisplay();
        return true;
    case 'r':  // restore a control point
        mSpline->SetControlPoint(4, Vector2f(mV2, mV2));
        mModified = false;
        OnDisplay();
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
