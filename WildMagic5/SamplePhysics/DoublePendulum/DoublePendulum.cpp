// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2011/07/28)

#include "DoublePendulum.h"
using namespace std;

WM5_WINDOW_APPLICATION(DoublePendulum);

//#define SINGLE_STEP

//----------------------------------------------------------------------------
DoublePendulum::DoublePendulum ()
    :
    WindowApplication2("SamplePhysics/DoublePendulum", 0, 0, 256, 256,
        Float4(1.0f, 1.0f, 1.0f, 1.0f))
{
    mSize = GetWidth();
}
//----------------------------------------------------------------------------
bool DoublePendulum::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    // Set up the physics module.
    mModule.Gravity = 10.0;
    mModule.Mass1 = 10.0;
    mModule.Mass2 = 20.0;
    mModule.Length1 = 100.0;
    mModule.Length2 = 100.0;
    mModule.JointX = (double)(mSize/2);
    mModule.JointY = (double)(mSize - 8);

    // Initialize the differential equations.
    double time = 0.0;
    double deltaTime = 0.01;
    double theta1 = 0.125*Mathd::PI;
    double theta1Dot = 0.0;
    double theta2 = 0.25*Mathd::PI;
    double theta2Dot = 0.0;
    mModule.Initialize(time, deltaTime, theta1, theta2, theta1Dot, theta2Dot);

    // Use right-handed display coordinates.
    DoFlip(true);

    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void DoublePendulum::OnIdle ()
{
#ifndef SINGLE_STEP
    mModule.Update();
    OnDisplay();
#endif
}
//----------------------------------------------------------------------------
void DoublePendulum::OnDisplay ()
{
    ClearScreen();

    ColorRGB black(0,0,0), gray(128,128,128), blue(0,0,255);

    double x1, y1, x2, y2;
    mModule.GetPositions(x1, y1, x2, y2);
    int ix1 = (int)(x1 + 0.5);
    int iy1 = (int)(y1 + 0.5);
    int ix2 = (int)(x2 + 0.5);
    int iy2 = (int)(y2 + 0.5);

    // Draw the axes.
    DrawLine(mSize/2, 0, mSize/2, mSize - 1, gray);
    DrawLine(0, 0, mSize - 1, 0, gray);

    // The pendulum joint.
    int jx = (int)(mModule.JointX + 0.5);
    int jy = (int)(mModule.JointY + 0.5);

    // Draw the pendulum rods.
    DrawLine(jx, jy, ix1, iy1, blue);
    DrawLine(ix1, iy1, ix2, iy2, blue);

    // Draw the pendulum joint.
    DrawCircle(jx, jy, 2, black, true);

    // Draw the pendulum masses.
    DrawCircle(ix1, iy1, 2, black, true);
    DrawCircle(ix2, iy2, 2, black, true);

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
bool DoublePendulum::OnKeyDown (unsigned char key, int x, int y)
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
        mModule.Update();
        OnDisplay();
        return true;
    }
#endif

    return false;
}
//----------------------------------------------------------------------------
