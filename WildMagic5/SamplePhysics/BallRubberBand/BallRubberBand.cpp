// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "BallRubberBand.h"

WM5_WINDOW_APPLICATION(BallRubberBand);

//----------------------------------------------------------------------------
BallRubberBand::BallRubberBand ()
    :
    WindowApplication2("SamplePhysics/BallRubberBand", 0, 0, 256, 256,
        Float4(1.0f, 1.0f, 1.0f, 1.0f))
{
}
//----------------------------------------------------------------------------
bool BallRubberBand::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    // Set up the physics module.
    mModule.SpringConstant = 16.0;
    mModule.Mass = 1.0;

    double time = 0.0;
    double deltaTime = 0.01;
    Vector2d position(96.0, 96.0);
    Vector2d velocity(64.0, 0.0);
    mModule.Initialize(time, deltaTime, position, velocity);

    const int imax = 128;
    mPosition.resize(imax);
    for (int i = 0; i < imax; ++i)
    {
        mPosition[i] = mModule.GetPosition();
        mModule.Update();
    }

    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void BallRubberBand::OnDisplay ()
{
    ClearScreen();

    const int halfSize = mWidth/2, sizeM1 = mWidth-1;
    const double dHalfSize = (double)halfSize;

    // Draw the coordinate axes.
    ColorRGB gray(192, 192, 192);
    DrawLine(0, halfSize, sizeM1, halfSize, gray);
    DrawLine(halfSize, 0, halfSize, sizeM1, gray);

    // Draw the ball's path.  The orbit starts in green, finishes in blue,
    // and is a blend of the two colors between.
    int numPositions = (int)mPosition.size();
    float invNumPositions = 1.0f/numPositions;
    for (int i = 0; i < numPositions-1; ++i)
    {
        float w = i*invNumPositions, omw = 1.0f - w;
        unsigned char blue = (unsigned char)(255.0f*omw);
        unsigned char green = (unsigned char)(255.0f*w);
        int x0 = (int)(mPosition[i].X() + dHalfSize + 0.5);
        int y0 = (int)(mPosition[i].Y() + dHalfSize + 0.5);
        int x1 = (int)(mPosition[i+1].X() + dHalfSize + 0.5);
        int y1 = (int)(mPosition[i+1].Y() + dHalfSize + 0.5);
        DrawLine(x0, y0, x1, y1, ColorRGB(0, green, blue));
    }

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
