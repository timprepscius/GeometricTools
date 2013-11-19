// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "NURBSCurveExample.h"

WM5_WINDOW_APPLICATION(NURBSCurveExample);

//----------------------------------------------------------------------------
NURBSCurveExample::NURBSCurveExample ()
    :
    WindowApplication2("SamplePhysics/NURBSCurveExample",0, 0, 256, 256,
        Float4(1.0f, 1.0f, 1.0f, 1.0f))
{
    mSpline = 0;
    mCircle = 0;
    mCtrlPoints = 0;
    mTargets = 0;
    mSize = GetWidth();
    mH = 0.5f*mSize;
    mD = 0.0625f*mSize;
    mSimTime = 0.0f;
    mSimDelta = 0.05f;
    mDrawControlPoints = false;
}
//----------------------------------------------------------------------------
bool NURBSCurveExample::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    InitialConfiguration();
    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void NURBSCurveExample::OnTerminate ()
{
    delete0(mSpline);
    delete0(mCircle);
    delete1(mCtrlPoints);
    delete1(mTargets);
    WindowApplication2::OnTerminate();
}
//----------------------------------------------------------------------------
void NURBSCurveExample::OnDisplay ()
{
    ClearScreen();

    ColorRGB curveColor(0,0,0);
    ColorRGB controlColor(128, 128, 128);

    int imax = 2048;
    int i, x, y;
    float t;
    Vector2f position;

    // Draw the spline.
    for (i = 0; i <= imax; ++i)
    {
        t = i/(float)imax;
        position = mSpline->GetPosition(t);
        x = (int)(position.X() + 0.5f);
        y = mSize - 1 - (int)(position.Y() + 0.5f);
        SetPixel(x, y, curveColor);
    }

    // Draw the circle.
    if (mCircle)
    {
        for (i = 0; i <= imax; ++i)
        {
            t = i/(float)imax;
            position = mCircle->GetPosition(t);
            x = (int)(position.X() + 0.5f);
            y = mSize - 1 - (int)(position.Y() + 0.5f);
            SetPixel(x, y, curveColor);
        }
    }

    // Draw the control points.
    if (mDrawControlPoints)
    {
        // Draw the spline control points.
        imax = mSpline->GetNumCtrlPoints();
        for (i = 0; i < imax; ++i)
        {
            const Vector2f& ctrl = mSpline->GetControlPoint(i);
            x = (int)(ctrl.X() + 0.5f);
            y = mSize - 1 -(int)(ctrl.Y() + 0.5f);
            SetThickPixel(x, y, 2, controlColor);
        }

        // Draw the circle control points.
        if (mCircle)
        {
            imax = mCircle->GetNumCtrlPoints();
            for (i = 0; i < imax; ++i)
            {
                const Vector2f& ctrl = mCircle->GetControlPoint(i);
                x = (int)(ctrl.X() + 0.5f);
                y = mSize - 1 - (int)(ctrl.Y() + 0.5f);
                SetThickPixel(x, y, 2, controlColor);
            }
        }
    }

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
bool NURBSCurveExample::OnKeyDown (unsigned char key, int x, int y)
{
    if (WindowApplication2::OnKeyDown(key, x, y))
    {
        return true;
    }

    switch (key)
    {
    case 'g':
        if (mSimTime <= 1.0f)
        {
            DoSimulation1();
        }
        else if (mSimTime <= 2.0f)
        {
            DoSimulation2();
        }
        else
        {
            InitialConfiguration();
        }
        return true;
    case '0':
        InitialConfiguration();
        OnDisplay();
        return true;
    case 'c':
        mDrawControlPoints = !mDrawControlPoints;
        OnDisplay();
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void NURBSCurveExample::DoSimulation1 ()
{
    mSimTime += mSimDelta;

    float t = mSimTime;
    float oneMinusT = 1.0f - t;
    int imax = mSpline->GetNumCtrlPoints();
    for (int i = 0; i < imax; ++i)
    {
        if (i == 2 || i == 10)
        {
            float s = Mathf::Pow(t, 1.5f);
            float oneMinusS = 1.0f - s;
            mSpline->SetControlPoint(i,
                oneMinusS*mCtrlPoints[i] + s*mTargets[i]);
        }
        else
        {
            mSpline->SetControlPoint(i,
                oneMinusT*mCtrlPoints[i] + t*mTargets[i]);
        }
    }

    OnDisplay();
}
//----------------------------------------------------------------------------
void NURBSCurveExample::DoSimulation2 ()
{
    mSimTime += mSimDelta;

    if (!mCircle)
    {
        NextConfiguration();
    }
    else
    {
        // The curve evolves to a line segment.
        float t = mSimTime - 1.0f;
        float oneMinusT = 1.0f - t;
        Vector2f ctrl = oneMinusT*mSpline->GetControlPoint(2) +
            t*mSpline->GetControlPoint(1);
        mSpline->SetControlPoint(2, ctrl);

        // The circle floats up a little bit.
        int imax = mCircle->GetNumCtrlPoints();
        for (int i = 0; i < imax; ++i)
        {
            ctrl = mCircle->GetControlPoint(i) + Vector2f::UNIT_Y;
            mCircle->SetControlPoint(i, ctrl);
        }
    }

    OnDisplay();
}
//----------------------------------------------------------------------------
void NURBSCurveExample::InitialConfiguration ()
{
    mSimTime = 0.0f;
    delete0(mSpline);
    delete0(mCircle);
    delete1(mCtrlPoints);
    delete1(mTargets);
    mCircle = 0;

    const int numCtrlPoints = 13;
    const int degree = 2;
    mCtrlPoints = new1<Vector2f>(numCtrlPoints);
    mTargets = new1<Vector2f>(numCtrlPoints);
    int i;
    for (i = 0; i < numCtrlPoints; ++i)
    {
        mCtrlPoints[i] = Vector2f(0.125f*mSize + 0.0625f*mSize*i,
            0.0625f*mSize);
    }

    mTargets[ 0] = mCtrlPoints[ 0];
    mTargets[ 1] = mCtrlPoints[ 6];
    mTargets[ 2] = Vector2f(mCtrlPoints[6].X(), mH - mD);
    mTargets[ 3] = Vector2f(mCtrlPoints[5].X(), mH - mD);
    mTargets[ 4] = Vector2f(mCtrlPoints[5].X(), mH);
    mTargets[ 5] = Vector2f(mCtrlPoints[5].X(), mH + mD);
    mTargets[ 6] = Vector2f(mCtrlPoints[6].X(), mH + mD);
    mTargets[ 7] = Vector2f(mCtrlPoints[7].X(), mH + mD);
    mTargets[ 8] = Vector2f(mCtrlPoints[7].X(), mH);
    mTargets[ 9] = Vector2f(mCtrlPoints[7].X(), mH - mD);
    mTargets[10] = Vector2f(mCtrlPoints[6].X(), mH - mD);
    mTargets[11] = mCtrlPoints[ 6];
    mTargets[12] = mCtrlPoints[12];

    float* weights = new1<float>(numCtrlPoints);
    for (i = 0; i < numCtrlPoints; ++i)
    {
        weights[i] = 1.0f;
    }

    const float modWeight = 0.3f;
    weights[3] = modWeight;
    weights[5] = modWeight;
    weights[7] = modWeight;
    weights[9] = modWeight;

    mSpline = new0 NURBSCurve2f(numCtrlPoints, mCtrlPoints, weights, degree,
        false, true);

    delete1(weights);
}
//----------------------------------------------------------------------------
void NURBSCurveExample::NextConfiguration ()
{
    delete1(mTargets);
    mTargets = 0;

    const int numCtrlPoints = 14;
    const int degree = 2;
    delete1(mCtrlPoints);
    mCtrlPoints = new1<Vector2f>(numCtrlPoints);
    float* weights = new1<float>(numCtrlPoints);

    // spline
    mCtrlPoints[0] = mSpline->GetControlPoint(0);
    mCtrlPoints[1] = mSpline->GetControlPoint(1);
    mCtrlPoints[2] = 0.5f*(mSpline->GetControlPoint(1) +
        mSpline->GetControlPoint(2));
    mCtrlPoints[3] = mSpline->GetControlPoint(11);
    mCtrlPoints[4] = mSpline->GetControlPoint(12);

    // circle
    int i, j;
    for (i = 2, j = 5; i <= 10; ++i, ++j)
    {
        mCtrlPoints[j] = mSpline->GetControlPoint(i);
    }

    for (i = 0; i < numCtrlPoints; ++i)
    {
        weights[i] = 1.0f;
    }

    weights[ 6] = mSpline->GetControlWeight(3);
    weights[ 8] = mSpline->GetControlWeight(5);
    weights[10] = mSpline->GetControlWeight(7);
    weights[12] = mSpline->GetControlWeight(9);

    delete0(mSpline);
    mSpline = new0 NURBSCurve2f(5, mCtrlPoints, weights, degree, false,
        true);

    mCircle = new0 NURBSCurve2f(9, &mCtrlPoints[5], &weights[5], degree,
        true, false);

    delete1(weights);
}
//----------------------------------------------------------------------------
