// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "RoughPlaneParticle1.h"

WM5_WINDOW_APPLICATION(RoughPlaneParticle1);

//#define SINGLE_STEP

//----------------------------------------------------------------------------
RoughPlaneParticle1::RoughPlaneParticle1 ()
    :
    WindowApplication2("SamplePhysics/RoughPlaneParticle1", 0, 0, 256, 256,
        Float4(1.0f, 1.0f, 1.0f, 1.0f))
{
    mSize = GetWidth();
    mContinueSolving = true;
}
//----------------------------------------------------------------------------
bool RoughPlaneParticle1::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    // Set up the physics module.
    mModule.Gravity = 10.0;
    mModule.Mass = 10.0;
    mModule.Friction = 1.0;
    mModule.Angle = 0.125*Mathd::PI;

    // Initialize the differential equations.
    double time = 0.0;
    double deltaTime = 1.0/60.0;
    double x = 0.0;
    double w = 0.0;
    double xDer = 10.0;
    double wDer = 40.0;
    mModule.Initialize(time, deltaTime, x, w, xDer, wDer);

    // Initialize the coefficients for the viscous friction solution.
    mR = mModule.Friction/mModule.Mass;
    mA0 = -xDer/mR;
    mA1 = x - mA0;
    mB1 = -mModule.Gravity*Mathd::Sin(mModule.Angle)/mR;
    mB2 = (wDer + mR*w - mB1)/mR;
    mB0 = w - mB2;

    // Save path of motion.
    mVFPositions.push_back(GetVFPosition(time));
    mSFPositions.push_back(Vector2d(x, w));

    // Use right-handed coordinates.
    DoFlip(true);

    // Mass drawing might extend outside the application window.
    ClampToWindow() = true;

    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void RoughPlaneParticle1::OnIdle ()
{
#ifndef SINGLE_STEP
    if (mContinueSolving)
    {
        mModule.Update();
        if (mModule.GetX() > 0.0 && mModule.GetW() <= 0.0)
        {
            mContinueSolving = false;
            return;
        }

        mVFPositions.push_back(GetVFPosition(mModule.GetTime()));
        mSFPositions.push_back(Vector2d(mModule.GetX(), mModule.GetW()));
        OnDisplay();
    }
#endif
}
//----------------------------------------------------------------------------
void RoughPlaneParticle1::OnDisplay ()
{
    ClearScreen();

    ColorRGB black(0, 0, 0);
    ColorRGB gray(128, 128, 128);
    ColorRGB blue(0, 0, 128);
    ColorRGB lightBlue(0, 0, 255);
    int x0, w0, x1, w1, i;
    Vector2d position;

    const double xScale = 1.25;
    const double wScale = 0.75;
    const int wOffset = 96;

    // Draw viscous friction path of motion.
    const int numVFPositions = (int)mVFPositions.size();
    position = mVFPositions[0];
    x0 = (int)(xScale*position.X() + 0.5);
    w0 = (int)(wScale*position.Y() + 0.5) + wOffset;
    x1 = x0;
    w1 = w0;
    for (i = 1; i < numVFPositions; ++i)
    {
        position = mVFPositions[i];
        x1 = (int)(xScale*position.X() + 0.5);
        w1 = (int)(wScale*position.Y() + 0.5) + wOffset;
        DrawLine(x0, w0, x1, w1, lightBlue);
        x0 = x1;
        w0 = w1;
    }

    // Draw the mass.
    SetThickPixel(x1, w1, 2, blue);

    // Draw static friction path of motion.
    const int numSFPositions = (int)mSFPositions.size();
    position = mSFPositions[0];
    x0 = (int)(xScale*position.X() + 0.5);
    w0 = (int)(wScale*position.Y() + 0.5) + wOffset;
    x1 = x0;
    w1 = w0;
    for (i = 1; i < numSFPositions; ++i)
    {
        position = mSFPositions[i];
        x1 = (int)(xScale*position.X() + 0.5);
        w1 = (int)(wScale*position.Y() + 0.5) + wOffset;
        DrawLine(x0, w0, x1, w1, gray);
        x0 = x1;
        w0 = w1;
    }

    // Draw the mass.
    SetThickPixel(x1, w1, 2, black);

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
bool RoughPlaneParticle1::OnKeyDown (unsigned char key, int x, int y)
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
        if (mContinueSolving)
        {
            mModule.Update();
            if (mModule.GetX() > 0.0 && mModule.GetW() <= 0.0)
            {
                mContinueSolving = false;
                return true;
            }
            mVFPositions.push_back(GetVFPosition(mModule.GetTime()));
            mSFPositions.push_back(Vector2d(mModule.GetX(), mModule.GetW()));
            OnDisplay();
        }
        return true;
    }
#endif

    return false;
}
//----------------------------------------------------------------------------
Vector2d RoughPlaneParticle1::GetVFPosition (double time)
{
    Vector2d position;

    double expValue = Mathd::Exp(-mR*time);
    position.X() = mA0*expValue + mA1;
    position.Y() = mB0*expValue + mB1*time + mB2;

    return position;
}
//----------------------------------------------------------------------------
