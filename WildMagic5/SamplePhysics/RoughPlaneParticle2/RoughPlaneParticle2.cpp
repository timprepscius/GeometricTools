// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "RoughPlaneParticle2.h"

WM5_WINDOW_APPLICATION(RoughPlaneParticle2);

//#define SINGLE_STEP

//----------------------------------------------------------------------------
RoughPlaneParticle2::RoughPlaneParticle2 ()
    :
    WindowApplication2("SamplePhysics/RoughPlaneParticle2", 0, 0, 256, 256,
        Float4(1.0f, 1.0f, 1.0f, 1.0f))
{
    mIteration = 0;
    mMaxIteration = 512;
    mSize = GetWidth();
}
//----------------------------------------------------------------------------
bool RoughPlaneParticle2::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    // Set up the physics module.
    mModule.Gravity = 10.0;
    mModule.Mass1 = 10.0;
    mModule.Mass2 = 20.0;
    mModule.Friction1 = 1.0;
    mModule.Friction2 = 1.0;

    // Initialize the differential equations.
    double time = 0.0;
    double deltaTime = 1.0/60.0;
    double x1 = 16.0;
    double y1 = 116.0;
    double x2 = 100.0;
    double y2 = 200.0;
    double xDot = 10.0;
    double yDot = -10.0;
    double thetaDot = 0.5;
    mModule.Initialize(time, deltaTime, x1, y1, x2, y2, xDot, yDot, thetaDot);

    // Use right-handed coordinates.
    DoFlip(true);

    // Drawing might extend outside the application window.
    ClampToWindow() = true;

    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void RoughPlaneParticle2::OnIdle ()
{
#ifndef SINGLE_STEP
    if (mIteration < mMaxIteration)
    {
        mModule.Update();
        OnDisplay();
        ++mIteration;
    }
#endif
}
//----------------------------------------------------------------------------
void RoughPlaneParticle2::OnDisplay ()
{
    ClearScreen();

    ColorRGB black(0, 0, 0);
    ColorRGB gray(128, 128, 128);
    ColorRGB blue(0, 0, 255);

    // Draw the rod.
    double x1, y1, x2, y2;
    mModule.Get(x1, y1, x2, y2);
    int iX1 = (int)(x1 + 0.5);
    int iY1 = (int)(y1 + 0.5);
    int iX2 = (int)(x2 + 0.5);
    int iY2 = (int)(y2 + 0.5);
    DrawLine(iX1, iY1, iX2, iY2, gray);

    // Draw the masses.
    SetThickPixel(iX1, iY1, 2, black);
    SetThickPixel(iX2, iY2, 2, black);

    // Draw the center of mass.
    int x = (int)(mModule.GetX() + 0.5);
    int y = (int)(mModule.GetY() + 0.5);
    SetThickPixel(x, y, 2, blue);

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
bool RoughPlaneParticle2::OnKeyDown (unsigned char key, int x, int y)
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
        if (mIteration < mMaxIteration)
        {
            mModule.Update();
            OnDisplay();
            ++mIteration;
        }
        return true;
    }
#endif

    return false;
}
//----------------------------------------------------------------------------
