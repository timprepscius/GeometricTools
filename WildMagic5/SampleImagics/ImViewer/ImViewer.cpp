// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.3 (2011/07/09)

#include "ImViewer.h"

WM5_WINDOW_APPLICATION(ImViewer);

//----------------------------------------------------------------------------
ImViewer::ImViewer ()
    :
    WindowApplication2("SampleImagics/ImViewer", 0, 0, 256, 256,
        Float4(0.9f, 0.9f, 0.9f, 1.0f)),
        mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
    mAllowResize = false;
    mMouseDown = false;
    mZ = 0;
    mBound = 0;
    mColorData = 0;
    mFloatData = 0;
    mNumSlicePixels = 0;
    memset(mPixelString,0,PIXEL_STRING_SIZE);
}
//----------------------------------------------------------------------------
bool ImViewer::OnPrecreate ()
{
    if (!WindowApplication2::OnPrecreate())
    {
        return false;
    }

    char* filename = 0;
    Application::TheCommand->GetFilename(filename);
    if (!filename)
    {
        // Input filename must be specified on the command line.
        return false;
    }

    // Load the image data.
    int rtti, sizeOf;
    char* data = 0;
    bool loaded = Lattice::LoadRaw(filename, mNumDimensions, mBound,
        mNumPixels, rtti, sizeOf, data);
    if (!loaded)
    {
        delete1(data);
        delete1(filename);
        return false;
    }

    // Replace the window title by the filename.
    mWindowTitle = std::string(filename);
    delete1(filename);

    // Set window size based on image size.  Adjust height to allow for
    // status bar.  The window width is chosen so that rows are multiples
    // of 4 bytes.
    mHeight = mBound[1];
    if (mHeight < 128)
    {
        mHeight = 128;
    }

    int statusHeight = 20;
    mHeight += statusHeight;

    mWidth = mBound[0];
    if (mWidth < 256)
    {
        mWidth = 256;
    }

    int remainder = (mWidth % 4);
    if (remainder > 0)
    {
        mWidth += 4 - remainder;
    }

    // The application image is stored differently depending on the input
    // image format.
    int i;
    if (rtti == Ergb5::GetRTTI())
    {
        mColorData = new1<ColorRGB>(mNumPixels);
        unsigned short* usData = (unsigned short*)data;
        for (i = 0; i < mNumPixels; i++)
        {
            unsigned char r = GetRed16(usData[i]);
            unsigned char g = GetGreen16(usData[i]);
            unsigned char b = GetBlue16(usData[i]);
            mColorData[i] = ColorRGB(r, g, b);
        }
    }
    else if (rtti == Ergb8::GetRTTI())
    {
        mColorData = new1<ColorRGB>(mNumPixels);
        unsigned int* uiData = (unsigned int*)data;
        for (i = 0; i < mNumPixels; i++)
        {
            unsigned char r = GetRed24(uiData[i]);
            unsigned char g = GetGreen24(uiData[i]);
            unsigned char b = GetBlue24(uiData[i]);
            mColorData[i] = ColorRGB(r, g, b);
        }
    }
    else if (rtti == Efloat::GetRTTI())
    {
        mFloatData = new1<float>(mNumPixels);
        memcpy(mFloatData, data, mNumPixels*sizeof(float));
    }
    else
    {
        mFloatData = new1<float>(mNumPixels);
        ImageConvert(mNumPixels, rtti, data, Efloat::GetRTTI(), mFloatData);
    }
    delete1(data);

    // Compute min, max and range of float images.
    if (mFloatData)
    {
        mMin = mFloatData[0];
        mMax = mMin;
        for (i = 1; i < mNumPixels; ++i)
        {
            if (mFloatData[i] < mMin)
            {
                mMin = mFloatData[i];
            }
            else if (mFloatData[i] > mMax)
            {
                mMax = mFloatData[i];
            }
        }
        mRange = mMax - mMin;
        if (mRange > 0.0f)
        {
            mInvRange = 1.0f/mRange;
        }
        else
        {
            mInvRange = 0.0f;
        }
    }

    // We only need memory to hold the active slice.
    mNumSlicePixels = mBound[0]*mBound[1];
    return true;
}
//----------------------------------------------------------------------------
bool ImViewer::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    CopySliceToScreen();
    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void ImViewer::OnTerminate ()
{
    delete1(mColorData);
    delete1(mFloatData);
    delete1(mBound);

    WindowApplication2::OnTerminate();
}
//----------------------------------------------------------------------------
bool ImViewer::OnSpecialKeyDown (int key, int x, int y)
{
    if (mNumDimensions != 3)
    {
        return false;
    }

    if (key == KEY_UP_ARROW)
    {
        // Up arrow pressed, go to next image slize.
        if (mZ < mBound[2] - 1)
        {
            ++mZ;
            CopySliceToScreen();
            if (mMouseDown)
            {
                ReadPixelValue(x, y);
            }
            OnDisplay();
        }
        return true;
    }

    if (key == KEY_DOWN_ARROW)
    {
        // Down arrow pressed, go to previous image slice.
        if (mZ > 0)
        {
            --mZ;
            CopySliceToScreen();
            if (mMouseDown)
            {
                ReadPixelValue(x, y);
            }
            OnDisplay();
        }
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
bool ImViewer::OnMouseClick (int button, int state, int x, int y,
    unsigned int)
{
    if (button == MOUSE_LEFT_BUTTON)
    {
        if (state == MOUSE_DOWN)
        {
            mMouseDown = true;
            ReadPixelValue(x, y);
            OnDisplay();
            return true;
        }
        if (state == MOUSE_UP)
        {
            mMouseDown = false;
            ReadPixelValue(-1, -1);
            OnDisplay();
            return true;
        }
    }
    return false;
}
//----------------------------------------------------------------------------
bool ImViewer::OnMotion (int, int x, int y, unsigned int)
{
    if (mMouseDown)
    {
        ReadPixelValue(x, y);
        OnDisplay();
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void ImViewer::CopySliceToScreen ()
{
    ClearScreen();

    int x, y;

    if (mFloatData)
    {
        if (mRange > 0.0f)
        {
            float* slice = &mFloatData[mZ*mNumSlicePixels];
            for (y = 0; y < mBound[1]; ++y)
            {
                for (x = 0; x < mBound[0]; ++x)
                {
                    int i = x + mBound[0]*y;
                    float fGray = 255.0f*(slice[i] - mMin)*mInvRange;
                    unsigned char gray = (unsigned char)(fGray);
                    mScreen[x + mScreenWidth*y] = ColorRGB(gray, gray, gray);
                }
            }
        }
    }
    else
    {
        ColorRGB* slice = &mColorData[mZ*mNumSlicePixels];
        for (y = 0; y < mBound[1]; ++y)
        {
            for (x = 0; x < mBound[0]; ++x)
            {
                int i = x + mBound[0]*y;
                mScreen[x + mScreenWidth*y] = slice[i];
            }
        }
    }
}
//----------------------------------------------------------------------------
void ImViewer::ReadPixelValue (int x, int y)
{
    if (0 <= x && x < mBound[0] && 0 <= y && y < mBound[1])
    {
        int index = x + mBound[0]*y;
        if (mFloatData)
        {
            float* slice = &mFloatData[mZ*mNumSlicePixels];
            float value = slice[index];
            if (mNumDimensions == 2)
            {
                sprintf(mPixelString, "(%d,%d) %f", x, y, value);
            }
            else
            {
                sprintf(mPixelString, "(%d,%d,%d) %f", x, y, mZ, value);
            }
        }
        else
        {
            ColorRGB* slice = &mColorData[mZ*mNumSlicePixels];
            ColorRGB value = slice[index];
            if (mNumDimensions == 2)
            {
                sprintf(mPixelString, "(%d,%d) r=%d g=%d b=%d", x, y,
                    (int)value.r, (int)value.g, (int)value.b);
            }
            else
            {
                sprintf(mPixelString, "(%d,%d,%d) r=%d g=%d b=%d", x, y,
                    mZ, (int)value.r, (int)value.g, (int)value.b);
            }
        }
    }
    else
    {
        int length = (int)strlen(mPixelString);
        if (length > 0)
        {
            memset(mPixelString, ' ', length);
        }
    }
}
//----------------------------------------------------------------------------
void ImViewer::ScreenOverlay ()
{
    if (mMouseDown)
    {
        mRenderer->Draw(4, mHeight - 4, mTextColor, mPixelString);
    }
}
//----------------------------------------------------------------------------
