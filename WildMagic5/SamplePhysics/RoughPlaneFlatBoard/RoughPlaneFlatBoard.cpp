// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "RoughPlaneFlatBoard.h"

WM5_WINDOW_APPLICATION(RoughPlaneFlatBoard);

//#define SINGLE_STEP

//----------------------------------------------------------------------------
RoughPlaneFlatBoard::RoughPlaneFlatBoard ()
    :
    WindowApplication2("SamplePhysics/RoughPlaneFlatBoard", 0, 0, 256, 256,
        Float4(1.0f, 1.0f, 1.0f, 1.0f))
{
    mSize = GetWidth();
}
//----------------------------------------------------------------------------
bool RoughPlaneFlatBoard::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    // Set up the physics module.
    mModule.MuGravity = 5.0;
    mModule.XLocExt = 16.0;
    mModule.YLocExt = 8.0;

    // Initialize the differential equations.
    double time = 0.0;
    double deltaTime = 1.0/60.0;
    double x = 20.0;
    double y = 230.0;
    double theta = 0.25*Mathd::PI;
    double xDer = 30.0;
    double yDer = -30.0;
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
void RoughPlaneFlatBoard::OnIdle ()
{
#ifndef SINGLE_STEP
    mModule.Update();
    OnDisplay();
#endif
}
//----------------------------------------------------------------------------
void RoughPlaneFlatBoard::OnDisplay ()
{
    ClearScreen();

    ColorRGB black(0, 0, 0);

    // Draw the board.
    double x00, y00, x10, y10, x11, y11, x01, y01;
    mModule.GetRectangle(x00, y00, x10, y10, x11, y11, x01, y01);
    int iX00 = (int)(x00 + 0.5);
    int iY00 = (int)(y00 + 0.5);
    int iX10 = (int)(x10 + 0.5);
    int iY10 = (int)(y10 + 0.5);
    int iX11 = (int)(x11 + 0.5);
    int iY11 = (int)(y11 + 0.5);
    int iX01 = (int)(x01 + 0.5);
    int iY01 = (int)(y01 + 0.5);
    DrawLine(iX00, iY00, iX10, iY10, black);
    DrawLine(iX10, iY10, iX11, iY11, black);
    DrawLine(iX11, iY11, iX01, iY01, black);
    DrawLine(iX01, iY01, iX00, iY00, black);

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
bool RoughPlaneFlatBoard::OnKeyDown (unsigned char key, int x, int y)
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
