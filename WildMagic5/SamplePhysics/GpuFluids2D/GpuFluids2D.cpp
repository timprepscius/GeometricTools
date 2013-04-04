// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "GpuFluids2D.h"

WM5_WINDOW_APPLICATION(GpuFluids2D);

//----------------------------------------------------------------------------
GpuFluids2D::GpuFluids2D ()
    :
    WindowApplication3("SamplePhysics/GpuFluids2D",0, 0, 512, 512,
        Float4(0.9f, 0.9f, 0.9f, 1.0f)),
        mTextColor(1.0f, 0.0f, 0.0f, 1.0f)
{
    mSmoke = 0;
    mDrawColored = false;
    mDrawVortices = true;
}
//----------------------------------------------------------------------------
bool GpuFluids2D::OnInitialize ()
{
    if (!WindowApplication3::OnInitialize())
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
    int numVortices = 64;

    mSmoke = new0 Smoke2D(mRenderer, x0, y0, x1, y1, dt, denViscosity,
        velViscosity, imax, jmax, numGaussSeidelIterations, numVortices);

    return true;
}
//----------------------------------------------------------------------------
void GpuFluids2D::OnTerminate ()
{
    delete0(mSmoke);
    WindowApplication3::OnTerminate();
}
//----------------------------------------------------------------------------
void GpuFluids2D::OnPreidle ()
{
    mSmoke->CreateScene();
    mSmoke->Initialize();
    mSmoke->UseVortexOverlay(mDrawVortices);
}
//----------------------------------------------------------------------------
void GpuFluids2D::OnIdle ()
{
    MeasureTime();

    if (mRenderer->PreDraw())
    {
        mSmoke->DoSimulationStep();

        DrawFrameRate(8, GetHeight()-8, mTextColor);
        mRenderer->PostDraw();
        mRenderer->DisplayColorBuffer();
    }

    UpdateFrameCount();
}
//----------------------------------------------------------------------------
bool GpuFluids2D::OnKeyDown (unsigned char key, int x, int y)
{
    if (WindowApplication3::OnKeyDown(key, x, y))
    {
        return true;
    }

    switch (key)
    {
    case '0':
        mSmoke->Initialize();
        return true;
    case 'c':
    case 'C':
        mDrawColored = !mDrawColored;
        mSmoke->UseColor(mDrawColored);
        return true;
    case 'v':
    case 'V':
        mDrawVortices = !mDrawVortices;
        mSmoke->UseVortexOverlay(mDrawVortices);
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
