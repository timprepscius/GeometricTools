// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "KeplerPolarForm.h"

WM5_WINDOW_APPLICATION(KeplerPolarForm);

//----------------------------------------------------------------------------
KeplerPolarForm::KeplerPolarForm ()
    :
    WindowApplication2("SamplePhysics/KeplerPolarForm", 0, 0, 256, 256,
        Float4(1.0f, 1.0f, 1.0f, 1.0f))
{
    mSize = GetWidth();
}
//----------------------------------------------------------------------------
bool KeplerPolarForm::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    // Set up the physics module.
    mModule.Gravity = 10.0;
    mModule.Mass = 1.0;

    double time = 0.0;
    double deltaTime = 0.01;
    double radius = 10.0;
    double theta = 0.25*Mathd::PI;
    double radiusDot = 0.1;
    double thetaDot = 0.1;
    mModule.Initialize(time, deltaTime, radius, theta, radiusDot, thetaDot);

    const int imax = (int)(mModule.GetPeriod()/deltaTime);
    mPositions.resize(imax);
    for (int i = 0; i < imax; ++i)
    {
        double x = 0.5*mSize + 10.0*radius*Mathd::Cos(theta);
        double y = 0.5*mSize + 10.0*radius*Mathd::Sin(theta);
        mPositions[i] = Vector2d(x, y);

        mModule.Update();

        time = mModule.GetTime();
        radius = mModule.GetRadius();
        radiusDot = mModule.GetRadiusDot();
        theta = mModule.GetTheta();
        thetaDot = mModule.GetThetaDot();
    }

    // All drawing is in flipped y-values to show the objects in right-handed
    // coordinates.
    DoFlip(true);

    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void KeplerPolarForm::OnDisplay ()
{
    ClearScreen();

    const int halfSize = (int)(0.5f*mSize);
    const int sizeM1 = (int)(mSize - 1.0f);

    // Draw the coordinate axes.
    ColorRGB gray(192, 192, 192);
    DrawLine(0, halfSize, sizeM1, halfSize, gray);
    DrawLine(halfSize, 0, halfSize, sizeM1, gray);

    // Draw a ray from the Sun's location to the initial point.
    int x = (int)(mPositions[1].X() + 0.5);
    int y = (int)(mPositions[1].Y() + 0.5);
    DrawLine(halfSize, halfSize, x, y, gray);

    // Draw the Sun's location.  The Sun is at the origin which happens to
    // be a focal point of the ellipse.
    SetThickPixel(halfSize, halfSize, 1, ColorRGB(255, 0, 0));

    // Draw Earth's orbit.  The orbit starts in green, finishes in blue, and
    // is a blend of the two colors between.
    const int numPositions = (int)mPositions.size();
    const float invNumPositions = 1.0f/(float)numPositions;
    for (int i = 1; i < numPositions; ++i)
    {
        float w = i*invNumPositions;
        float oneMinusW = 1.0f - w;
        unsigned char blue = (unsigned char)(255.0f*oneMinusW);
        unsigned char green = (unsigned char)(255.0f*w);
        x = (int)(mPositions[i].X() + 0.5);
        y = (int)(mPositions[i].Y() + 0.5);
        SetPixel(x, y, ColorRGB(0, green, blue));
    }

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
