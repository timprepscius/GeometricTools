// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Fluids2D.h"

WM5_WINDOW_APPLICATION(Fluids2D);

//----------------------------------------------------------------------------
Fluids2D::Fluids2D ()
    :
    WindowApplication2("SamplePhysics/Fluids2D",0, 0, 512, 512,
        Float4(0.9f, 0.9f, 0.9f, 1.0f)),
        mTextColor(1.0f, 1.0f, 0.0f, 1.0f)
{
    mSmoke = 0;

    ColorRGB key[9] =
    {
        ColorRGB(  0,   0,   0),  // black
        ColorRGB(128,  64,  64),  // brown
        ColorRGB(128,   0, 255),  // violet
        ColorRGB(  0,   0, 255),  // blue
        ColorRGB(  0, 255,   0),  // green
        ColorRGB(255, 255,   0),  // yellow
        ColorRGB(255, 128,   0),  // orange
        ColorRGB(255,   0,   0),  // red
        ColorRGB(255, 255, 255)   // white
    };

    for (int i = 0, j = 0; i < 8; ++i)
    {
        for (int k = 0; k < 32; ++k, ++j)
        {
            float t = k/32.0f;
            float omt = 1.0f - t;
            mColor[j].r = (unsigned char)(omt*key[i].r + t*key[i+1].r);
            mColor[j].g = (unsigned char)(omt*key[i].g + t*key[i+1].g);
            mColor[j].b = (unsigned char)(omt*key[i].b + t*key[i+1].b);
        }
    }

    mSingleStep = false;
    mDrawColored = false;
    mDrawVortices = true;
}
//----------------------------------------------------------------------------
bool Fluids2D::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    float x0 = 0.0f;
    float y0 = 0.0f;
    float x1 = 1.0f;
    float y1 = 1.0f;
    float dt = 0.001f;
    float denViscosity = 0.0001f;
    float velViscosity = 0.0001f;
    int imax = 255;
    int jmax = 255;
    int numGaussSeidelIterations = 32;
    bool densityDirichlet = true;
    int numVortices = 64;
    mSmoke = new0 Smoke2D<float>(x0, y0, x1, y1, dt, denViscosity,
        velViscosity, imax, jmax, numGaussSeidelIterations, densityDirichlet,
        numVortices);

    mSmoke->Initialize();
    DoFlip(true);
    OnDisplay();

    return true;
}
//----------------------------------------------------------------------------
void Fluids2D::OnTerminate ()
{
    delete0(mSmoke);
    WindowApplication2::OnTerminate();
}
//----------------------------------------------------------------------------
void Fluids2D::OnDisplay ()
{
    // Draw the density values.
    int imax = mSmoke->GetIMax();
    int jmax = mSmoke->GetJMax();
    float** density = mSmoke->GetDensity();
    for (int j = 0; j <= jmax; ++j)
    {
        for (int i = 0; i <= imax; ++i)
        {
            float value = density[j][i];
            if (value > 1.0f)
            {
                value = 1.0f;
            }

            ColorRGB color;
            if (mDrawColored)
            {
                color = mColor[(int)(255.0f*value)];
            }
            else
            {
                unsigned char gray = (unsigned char)(255.0f*value);
                color.r = gray;
                color.g = gray;
                color.b = gray;
            }

            SetPixel(2*i, 2*j, color);
            SetPixel(2*i+1, 2*j, color);
            SetPixel(2*i, 2*j+1, color);
            SetPixel(2*i+1, 2*j+1, color);
        }
    }

    if (mDrawVortices)
    {
        // Draw the vortex centers, magenta for counterclockwise and cyan for
        // clockwise.
        const int numActive = mSmoke->GetNumActiveVortices();
        float xMin = mSmoke->GetX0();
        float yMin = mSmoke->GetY0();
        float invXRange = 1.0f/(mSmoke->GetX1() - mSmoke->GetX0());
        float invYRange = 1.0f/(mSmoke->GetY1() - mSmoke->GetY0());
        for (int k = 0; k < numActive; ++k)
        {
            Vector2f center = mSmoke->GetVortexCenter(k);
            int x = (int)((GetWidth()-1)*(center.X() - xMin)*invXRange);
            int y = (int)((GetHeight()-1)*(center.Y() - yMin)*invYRange);

            ColorRGB color;
            if (mSmoke->GetVortexAmplitude(k) > 0.0f)
            {
                color.r = 255;
                color.g = 0;
                color.b = 255;
            }
            else
            {
                color.r = 0;
                color.g = 255;
                color.b = 255;
            }

            SetThickPixel(x, y, 1, color);
        }
    }

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
void Fluids2D::ScreenOverlay ()
{
    DrawFrameRate(8, GetHeight()-8, mTextColor);
}
//----------------------------------------------------------------------------
void Fluids2D::OnIdle ()
{
    MeasureTime();

    if (!mSingleStep)
    {
        mSmoke->DoSimulationStep();
        OnDisplay();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool Fluids2D::OnKeyDown (unsigned char key, int x, int y)
{
    if (WindowApplication2::OnKeyDown(key, x, y))
    {
        return true;
    }

    switch (key)
    {
    case '0':
        mSmoke->Initialize();
        OnDisplay();
        return true;
    case ' ':  // simulate
        if (mSingleStep)
        {
            mSmoke->DoSimulationStep();
            OnDisplay();
        }
        return true;
    case 's':
    case 'S':
        mSingleStep = !mSingleStep;
        return true;
    case 'c':
    case 'C':
        mDrawColored = !mDrawColored;
        return true;
    case 'v':
    case 'V':
        mDrawVortices = !mDrawVortices;
        return true;
    case '+':
    case '=':
    {
        int numActive = mSmoke->GetNumActiveVortices();
        if (numActive < mSmoke->GetNumVortices())
        {
            mSmoke->SetNumActiveVortices(numActive + 1);
        }
        return true;
    }
    case '-':
    case '_':
    {
        int numActive = mSmoke->GetNumActiveVortices();
        if (numActive > 0)
        {
            mSmoke->SetNumActiveVortices(numActive - 1);
        }
        return true;
    }
    case 'g':
    {
        float gravity = mSmoke->GetGravity() - 0.1f;
        if (gravity < 0.0f)
        {
            gravity = 0.0f;
        }
        mSmoke->SetGravity(gravity);
        return true;
    }
    case 'G':
        mSmoke->SetGravity(mSmoke->GetGravity() + 0.1f);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
