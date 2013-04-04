// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "IntersectingRectangles.h"

WM5_WINDOW_APPLICATION(IntersectingRectangles);

// #define SINGLE_STEP

//----------------------------------------------------------------------------
IntersectingRectangles::IntersectingRectangles ()
    :
    WindowApplication2("SamplePhysics/IntersectingRectangles", 0, 0, 256, 256,
        Float4(1.0f, 1.0f, 1.0f, 1.0f))
{
    mManager = 0;
    mLastIdle = 0.0f;
    mSize = GetWidth();
    mMouseDown = false;
}
//----------------------------------------------------------------------------
bool IntersectingRectangles::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    for (int i = 0; i < 16; ++i)
    {
        float xmin = Mathf::IntervalRandom(0.125f*mSize, 0.875f*mSize);
        float xmax = xmin + Mathf::IntervalRandom(4.0f, 32.0f);
        float ymin = Mathf::IntervalRandom(0.125f*mSize, 0.875f*mSize);
        float ymax = ymin + Mathf::IntervalRandom(4.0f, 32.0f);
        mRectangles.push_back(AxisAlignedBox2f(xmin, xmax, ymin, ymax));
    }

    mManager = new0 RectangleManagerf(mRectangles);

    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void IntersectingRectangles::OnTerminate ()
{
    delete0(mManager);
    WindowApplication2::OnTerminate();
}
//----------------------------------------------------------------------------
void IntersectingRectangles::OnIdle ()
{
#ifndef SINGLE_STEP
    float currIdle = (float)GetTimeInSeconds();
    float diff = currIdle - mLastIdle;
    if (diff >= 1.0f/30.0f)
    {
        ModifyRectangles();
        OnDisplay();
        mLastIdle = currIdle;
    }
#endif
}
//----------------------------------------------------------------------------
void IntersectingRectangles::OnDisplay ()
{
    ClearScreen();
    DrawRectangles();
    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
bool IntersectingRectangles::OnKeyDown (unsigned char key, int x, int y)
{
    if (WindowApplication2::OnKeyDown(key, x, y))
    {
        return true;
    }

#ifdef SINGLE_STEP
    switch (key)
    {
    case 'g':
    case 'G':
        ModifyRectangles();
        OnDisplay();
        return true;
    }
#endif

    return false;
}
//----------------------------------------------------------------------------
void IntersectingRectangles::ModifyRectangles ()
{
    const int numRectangles = (int)mRectangles.size();
    for (int i = 0; i < numRectangles; ++i)
    {
        AxisAlignedBox2f rectangle = mRectangles[i];

        float dx = Mathf::IntervalRandom(-4.0f, 4.0f);
        if (0.0f <= rectangle.Min[0] + dx && rectangle.Max[0] + dx < mSize)
        {
            rectangle.Min[0] += dx;
            rectangle.Max[0] += dx;
        }

        float dy = Mathf::IntervalRandom(-4.0f, 4.0f);
        if (0.0f <= rectangle.Min[1] + dy && rectangle.Max[1] + dy < mSize)
        {
            rectangle.Min[1] += dy;
            rectangle.Max[1] += dy;
        }

        mManager->SetRectangle(i, rectangle);
    }

    mManager->Update();
}
//----------------------------------------------------------------------------
void IntersectingRectangles::DrawRectangles ()
{
    ColorRGB gray(192,192,192), black(0,0,0), red(255,0,0);
    int i, xmin, xmax, ymin, ymax;
    const int numRectangles = (int)mRectangles.size();
    for (i = 0; i < numRectangles; ++i)
    {
        const AxisAlignedBox2f& rectangle = mRectangles[i];
        xmin = (int)rectangle.Min[0];
        xmax = (int)rectangle.Max[0];
        ymin = (int)rectangle.Min[1];
        ymax = (int)rectangle.Max[1];
        DrawRectangle(xmin, ymin, xmax, ymax, gray, true);
        DrawRectangle(xmin, ymin, xmax, ymax, black);
    }

    const std::set<EdgeKey>& overlap = mManager->GetOverlap();
    std::set<EdgeKey>::const_iterator iter = overlap.begin();
    std::set<EdgeKey>::const_iterator end = overlap.end();
    for (/**/; iter != end; ++iter)
    {
        int i0 = iter->V[0];
        int i1 = iter->V[1];
        const AxisAlignedBox2f& rectangle0 = mRectangles[i0];
        const AxisAlignedBox2f& rectangle1 = mRectangles[i1];
        AxisAlignedBox2f intr;
        if (rectangle0.FindIntersection(rectangle1, intr))
        {
            xmin = (int)intr.Min[0];
            xmax = (int)intr.Max[0];
            ymin = (int)intr.Min[1];
            ymax = (int)intr.Max[1];
            DrawRectangle(xmin, ymin, xmax, ymax, red, true);
            DrawRectangle(xmin, ymin, xmax, ymax, black);
        }
    }
}
//----------------------------------------------------------------------------
