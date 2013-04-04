// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "RoughPlaneThinRod2.h"

WM5_WINDOW_APPLICATION(RoughPlaneThinRod2);

//#define SINGLE_STEP

//----------------------------------------------------------------------------
RoughPlaneThinRod2::RoughPlaneThinRod2 ()
    :
    WindowApplication2("SamplePhysics/RoughPlaneThinRod2", 0, 0, 256, 256,
        Float4(1.0f, 1.0f, 1.0f, 1.0f))
{
    mSize = GetWidth();
}
//----------------------------------------------------------------------------
bool RoughPlaneThinRod2::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    // Same starting values as in RoughPlaneParticle2.
    double x1 = 16.0;
    double y1 = 116.0;
    double x2 = 100.0;
    double y2 = 200.0;
    double xDelta = x2 - x1;
    double yDelta = y2 - y1;

    // Set up the physics module.
    mModule.Length = Mathd::Sqrt(xDelta*xDelta + yDelta*yDelta);
    mModule.MuGravity = 5.0;  // mu*g = c/delta0 from RoughPlaneThinRod1

    // Initialize the differential equations.
    double time = 0.0;
    double deltaTime = 1.0/60.0;
    double x = 0.5*(x1 + x2);
    double y = 0.5*(y1 + y2);
    double theta = Mathd::ATan2(yDelta, xDelta);
    double xDer = 10.0;
    double yDer = -10.0;
    double thetaDer = 4.0;
    mModule.Initialize(time, deltaTime, x, y, theta, xDer, yDer, thetaDer);

    // Use right-handed coordinates.
    DoFlip(true);

    // Drawing might extend outside the application window.
    ClampToWindow() = true;

    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void RoughPlaneThinRod2::OnIdle ()
{
#ifndef SINGLE_STEP
    mModule.Update();
    OnDisplay();
#endif
}
//----------------------------------------------------------------------------
void RoughPlaneThinRod2::OnDisplay ()
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
bool RoughPlaneThinRod2::OnKeyDown (unsigned char key, int x, int y)
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
