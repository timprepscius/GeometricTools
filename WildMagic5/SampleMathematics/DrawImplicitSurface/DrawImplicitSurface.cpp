// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2012/07/07)

#include "DrawImplicitSurface.h"
#include "Function.h"

WM5_WINDOW_APPLICATION(DrawImplicitSurface);

//----------------------------------------------------------------------------
DrawImplicitSurface::DrawImplicitSurface ()
    :
    WindowApplication2("SampleMathematics/DrawImplicitSurface", 0, 0, 256,
        256, Float4(1.0f, 1.0f, 1.0f, 1.0f)),
    mTracer(F, DF, 256, 256)
{
    mNumSamples = 100;
    mBlur = false;
    mSize = GetWidth();
}
//----------------------------------------------------------------------------
bool DrawImplicitSurface::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    // Initialize camera and view frustum.
    mTracer.Location = Vector3f(2.0f, 0.0f, 0.0f);
    mTracer.DVector = Vector3f(-1.0f, 0.0f, 0.0f);
    mTracer.UVector = Vector3f(0.0f, 1.0f, 0.0f);
    mTracer.RVector = mTracer.DVector.Cross(mTracer.UVector);
    mTracer.Near = 0.1f;
    mTracer.Far = 10.0f;
    mTracer.HalfWidth = 2.0f*mTracer.Near;  // 90 degree horizontal FOV
    mTracer.HalfHeight = 2.0f*mTracer.Near;  // 90 degree vertical FOV

    // The light direction will be the camera direction so that we can see
    // the surface from all camera locations.

    // Draw the level surface.
    mTracer.DrawSurface(mNumSamples, mTracer.DVector, mBlur);
    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void DrawImplicitSurface::OnDisplay ()
{
    ClearScreen();

    const float* image = mTracer.GetImage();
    for (int y = 0, i = 0; y < mTracer.GetHeight(); ++y)
    {
        for (int x = 0; x < mTracer.GetWidth(); ++x, ++i)
        {
            unsigned char value = (unsigned char)(255.0f*image[i]);
            SetPixel(x, y, ColorRGB(value, value, value));
        }
    }

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
bool DrawImplicitSurface::OnKeyDown (unsigned char key, int x, int y)
{
    if (WindowApplication2::OnKeyDown(key, x, y))
    {
        return true;
    }

    switch (key)
    {
    case '+':  // increase ray sample size
    case '=':
        mNumSamples += 100;
        mTracer.DrawSurface(mNumSamples, mTracer.DVector, mBlur);
        OnDisplay();
        return true;
    case '-':  // decrease ray sample size
    case '_':
        if (mNumSamples > 100)
        {
            mNumSamples -= 100;
            mTracer.DrawSurface(mNumSamples, mTracer.DVector, mBlur);
            OnDisplay();
        }
        return true;
    case 'b':  // toggle blur of output image
    case 'B':
        mBlur = !mBlur;
        mTracer.DrawSurface(mNumSamples, mTracer.DVector, mBlur);
        OnDisplay();
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
bool DrawImplicitSurface::OnSpecialKeyDown (int key, int, int)
{
    // TODO:  These are chosen for the specific functions in this
    // application.  Allow the application to modify these, either by key
    // strokes or automatically.
    const float trnDelta = 0.25f, rotDelta = 0.1f;
    bool moved = false;
    float length;
    Matrix3f rot;

    if (key == KEY_UP_ARROW)
    {
        // Translate forward in camera direction.
        mTracer.Location += trnDelta*mTracer.DVector;
        moved = true;
    }
    else if (key == KEY_DOWN_ARROW)
    {
        // Translate backward in camera direction.
        mTracer.Location -= trnDelta*mTracer.DVector;
        moved = true;
    }
    else if (key == KEY_F1)
    {
        // Rotate about camera right, move up on view sphere.
        length = mTracer.Location.Length();
        rot.MakeRotation(mTracer.RVector, rotDelta);
        mTracer.DVector = rot*mTracer.DVector;
        mTracer.UVector = rot*mTracer.UVector;
        mTracer.Location = -length*mTracer.DVector;
        moved = true;
    }
    else if (key == KEY_F2)
    {
        // Rotate about camera right, move down on view sphere.
        length = mTracer.Location.Length();
        rot.MakeRotation(mTracer.RVector, -rotDelta);
        mTracer.DVector = rot*mTracer.DVector;
        mTracer.UVector = rot*mTracer.UVector;
        mTracer.Location = -length*mTracer.DVector;
        moved = true;
    }
    else if (key == KEY_F3)
    {
        // Rotate about camera up, move right on view sphere.
        length = mTracer.Location.Length();
        rot.MakeRotation(mTracer.UVector, rotDelta);
        mTracer.DVector = rot*mTracer.DVector;
        mTracer.RVector = rot*mTracer.RVector;
        mTracer.Location = -length*mTracer.DVector;
        moved = true;
    }
    else if (key == KEY_F4)
    {
        // Rotate about camera up, move left on view sphere.
        length = mTracer.Location.Length();
        rot.MakeRotation(mTracer.UVector, -rotDelta);
        mTracer.DVector = rot*mTracer.DVector;
        mTracer.RVector = rot*mTracer.RVector;
        mTracer.Location = -length*mTracer.DVector;
        moved = true;
    }
    else if (key == KEY_F5)
    {
        // Rotate about camera direction, roll counterclockwise.
        length = mTracer.Location.Length();
        rot.MakeRotation(mTracer.DVector, rotDelta);
        mTracer.UVector = rot*mTracer.UVector;
        mTracer.RVector = rot*mTracer.RVector;
        moved = true;
    }
    else if (key == KEY_F6)
    {
        // Rotate about camera direction, roll clockwise.
        length = mTracer.Location.Length();
        rot.MakeRotation(mTracer.DVector, -rotDelta);
        mTracer.UVector = rot*mTracer.UVector;
        mTracer.RVector = rot*mTracer.RVector;
        moved = true;
    }

    if (moved)
    {
        mTracer.DrawSurface(mNumSamples, mTracer.DVector, mBlur);
        OnDisplay();
    }

    return true;
}
//----------------------------------------------------------------------------
