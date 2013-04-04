// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "VolumeRenderer.h"

WM5_WINDOW_APPLICATION(VolumeRenderer);

#define TEST_VR

//----------------------------------------------------------------------------
VolumeRenderer::VolumeRenderer ()
    :
    WindowApplication2("SampleImagics/VolumeRenderer", 0, 0, 0, 0,
        Float4(1.0f, 1.0f, 1.0f, 1.0f))
{
    mButtonDown = false;
    mBound = 0;
    mGamma = 0.25f;
}
//----------------------------------------------------------------------------
bool VolumeRenderer::OnPrecreate ()
{
    if (!WindowApplication2::OnPrecreate())
    {
        return false;
    }

#ifdef TEST_VR
    std::string imageName = Environment::GetPathR("Molecule.im");
    size_t length = strlen(imageName.c_str());
    char* filename = new1<char>(length + 1);
    strcpy(filename, imageName.c_str());
#else
    char* filename = 0;
    TheCommand->GetFilename(filename);
    if (!filename)
    {
        // Input filename must be specified on the command line.
        return false;
    }
#endif

    // Load image, must be 3D and pixels must be unsigned char.
    int numDimensions, numPixels, rtti, sizeOf;
    int* bounds = 0;
    char* data = 0;
    bool loaded = Lattice::LoadRaw(filename, numDimensions, bounds,
        numPixels, rtti, sizeOf, data);
    if (!loaded || numDimensions != 3 || rtti != Euchar::GetRTTI())
    {
        delete1(data);
        delete1(filename);
        return false;
    }

    ImageUChar3D* image = new0 ImageUChar3D(bounds[0], bounds[1], bounds[2],
        (Euchar*)data);

    // Get the maximum bound.
    int maxBound = image->GetBound(0);
    if (image->GetBound(1) > maxBound)
    {
        maxBound = image->GetBound(1);
    }
    if (image->GetBound(2) > maxBound)
    {
        maxBound = image->GetBound(2);
    }
    mBound = 2*maxBound;
    mHBound = (float)maxBound;

    mRT = new0 RayTrace(image, mGamma);
    delete0(image);

    // Resize application window.
    mWidth = mBound;
    mHeight = mBound;

    delete1(filename);
    delete1(bounds);
    return true;
}
//----------------------------------------------------------------------------
bool VolumeRenderer::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    mRT->Trace(1);
    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void VolumeRenderer::OnTerminate ()
{
    delete0(mRT);
    WindowApplication2::OnTerminate();
}
//----------------------------------------------------------------------------
void VolumeRenderer::OnDisplay ()
{
    mRT->DrawWireFrame();

    for (int y = 0; y < mBound; ++y)
    {
        for (int x = 0; x < mBound; ++x)
        {
            int i = x + mBound*y;
            unsigned int value = mRT->GetRendered(i);
            unsigned char r = GetRed24(value);
            unsigned char g = GetGreen24(value);
            unsigned char b = GetBlue24(value);
            SetPixel(x, y, ColorRGB(r, g, b));
        }
    }

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
bool VolumeRenderer::OnMouseClick (int button, int state, int x, int y,
    unsigned int)
{
    if (button != MOUSE_LEFT_BUTTON)
    {
        return false;
    }

    if (state == MOUSE_DOWN)
    {
        mButtonDown = true;
        mX0 = (x - mHBound)/mHBound;
        mY0 = (y - mHBound)/mHBound;
    }
    else
    {
        mButtonDown = false;
        mX1 = (x - mHBound)/mHBound;
        mY1 = (y - mHBound)/mHBound;

        if (mRT->MoveTrackBall(mX0, mY0, mX1, mY1))
        {
            mRT->Trace(1);
            OnDisplay();
        }
    }

    return true;
}
//----------------------------------------------------------------------------
bool VolumeRenderer::OnMotion (int, int x, int y, unsigned int)
{
    if (!mButtonDown)
    {
        return false;
    }

    // Save old frame.
    float saveFrame[3][3];
    int row, col;
    for (row = 0; row < 3; ++row)
    {
        for (col = 0; col < 3; ++col)
        {
            saveFrame[row][col] = mRT->Frame(row,col);
        }
    }
        
    mX1 = (x - mHBound)/mHBound;
    mY1 = (y - mHBound)/mHBound;
    if (mRT->MoveTrackBall(mX0, mY0, mX1, mY1))
    {
        // Trace every second ray, then fill in with nearest neighbor values.
        mRT->Trace(2);
        OnDisplay();

        // Restore old frame.
        for (row = 0; row < 3; ++row)
        {
            for (col = 0; col < 3; ++col)
            {
                mRT->Frame(row,col) = saveFrame[row][col];
            }
        }
    }

    return true;
}
//----------------------------------------------------------------------------
bool VolumeRenderer::OnKeyDown (unsigned char key, int x, int y)
{
    if (WindowApplication2::OnKeyDown(key, x, y))
    {
        return true;
    }

    switch (key)
    {
    case '+':
        mGamma /= 1.1f;
        mRT->Correction(mGamma);
        OnDisplay();
        return true;
    case '-':
        mGamma *= 1.1f;
        mRT->Correction(mGamma);
        OnDisplay();
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
