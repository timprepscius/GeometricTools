// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.3 (2012/07/04)

#include "WmtfViewer.h"

WM5_WINDOW_APPLICATION(WmtfViewer);

//----------------------------------------------------------------------------
WmtfViewer::WmtfViewer ()
    :
    WindowApplication2("Tools/WmtfViewer", 0, 0, 256, 256,
        Float4(0.9f, 0.9f, 0.9f, 1.0f)),
        mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
    mAllowResize = false;
    mTexture = 0;
    mFormat = Texture::TF_NONE;
    mNumTexels = 0;
    mXDim = 0;
    mYDim = 0;
    mTexels = 0;
    mImage = 0;
    mSliceIndex = 0;
    mRGBMin = 0.0f;
    mRGBMax = 0.0f;
    mInvRGBRange = 0.0f;
    mAMin = 0.0f;
    mAMax = 0.0f;
    mInvARange = 0.0f;
    mAlphaActive = false;
    mMouseDown = false;
    mPixelString[0] = ' ';
    mPixelString[1] = 0;

    mSFunction[Texture::TF_NONE] = 0;
    mSFunction[Texture::TF_R5G6B5] = &WmtfViewer::SetStrR5G6B5;
    mSFunction[Texture::TF_A1R5G5B5] = &WmtfViewer::SetStrA1R5G5B5;
    mSFunction[Texture::TF_A4R4G4B4] = &WmtfViewer::SetStrA4R4G4B4;
    mSFunction[Texture::TF_A8] = &WmtfViewer::SetStrA8;
    mSFunction[Texture::TF_L8] = &WmtfViewer::SetStrL8;
    mSFunction[Texture::TF_A8L8] = &WmtfViewer::SetStrA8L8;
    mSFunction[Texture::TF_R8G8B8] = &WmtfViewer::SetStrR8G8B8;
    mSFunction[Texture::TF_A8R8G8B8] = &WmtfViewer::SetStrA8R8G8B8;
    mSFunction[Texture::TF_A8B8G8R8] = &WmtfViewer::SetStrA8B8G8R8;
    mSFunction[Texture::TF_L16] = &WmtfViewer::SetStrL16;
    mSFunction[Texture::TF_G16R16] = &WmtfViewer::SetStrG16R16;
    mSFunction[Texture::TF_A16B16G16R16] = &WmtfViewer::SetStrA16B16G16R16;
    mSFunction[Texture::TF_R16F] = &WmtfViewer::SetStrR16F;
    mSFunction[Texture::TF_G16R16F] = &WmtfViewer::SetStrG16R16F;
    mSFunction[Texture::TF_A16B16G16R16F] = &WmtfViewer::SetStrA16B16G16R16F;
    mSFunction[Texture::TF_R32F] = &WmtfViewer::SetStrR32F;
    mSFunction[Texture::TF_G32R32F] = &WmtfViewer::SetStrG32R32F;
    mSFunction[Texture::TF_A32B32G32R32F] = &WmtfViewer::SetStrA32B32G32R32F;
    mSFunction[Texture::TF_DXT1] = 0;
    mSFunction[Texture::TF_DXT3] = 0;
    mSFunction[Texture::TF_DXT5] = 0;
    mSFunction[Texture::TF_D24S8] = 0;
}
//----------------------------------------------------------------------------
bool WmtfViewer::OnPrecreate ()
{
    // This function allows the application window to be of an appropriate
    // size so that the input image fits.

    if (!WindowApplication2::OnPrecreate())
    {
        return false;
    }

    char* filename = 0;
    TheCommand->GetFilename(filename);
    if (!filename)
    {
        // The input filename must be specified on the command line.
        return false;
    }

    // Load the image.
    Texture* texture = Texture::LoadWMTF(filename);
    if (!texture)
    {
        // Texture does not exist.
        return false;
    }

    // Replace the window title by the filename.
    mWindowTitle = std::string(filename);
    delete1(filename);

    mTexture = DynamicCast<Texture2D>(texture);
    if (!mTexture)
    {
        // Texture must be 2D (and not a cube map).  TODO: Support other
        // types of textures.
        return false;
    }

    // Mipmaps not supported for display.  TODO: Support mipmaps.
    mWidth = mTexture->GetDimension(0, 0);
    mHeight = mTexture->GetDimension(1, 0);
    if (mWidth > 1024 || mHeight > 1024)
    {
        // The texture is too large to display.  TODO: Support arbitrary
        // size images (by allowing hand-cursor moving).
        return false;
    }

    // Set window size based on image size.  Adjust height to allow for
    // status bar.  The window width is chosen so that rows are multiples
    // of 4 bytes.
    if (mHeight < 128)
    {
        mHeight = 128;
    }

    int statusHeight = 64;
    mHeight += statusHeight;

    if (mWidth < 512)
    {
        mWidth = 512;
    }

    int remainder = (mWidth % 4);
    if (remainder > 0)
    {
        mWidth += 4 - remainder;
    }

    return true;
}
//----------------------------------------------------------------------------
bool WmtfViewer::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    // Get the texture information.
    mFormat = mTexture->GetFormat();
    mXDim = mTexture->GetWidth();
    mYDim = mTexture->GetHeight();
    mNumTexels = mXDim*mYDim;
    mTexels = mTexture->GetData(0);

    // The image is stored in right-handed screen coordinates, where the
    // origin is the lower-left corner of the screen.  Reflect the image
    // to display in left-handed coordinates.
    unsigned char save[16];  // 16 = maximum number of bytes per pixel
    unsigned char* data = (unsigned char*)mTexels;
    int bytesPerPixel = mTexture->GetPixelSize();
    for (int y = 0; y < mYDim/2; ++y)
    {
        for (int x = 0; x < mXDim; ++x)
        {
            int isrc = bytesPerPixel*(x + mXDim*y);
            int itrg = bytesPerPixel*(x + mXDim*(mYDim - 1 - y));
            unsigned char* srcData = &data[isrc];
            unsigned char* trgData = &data[itrg];
            memcpy(save, srcData, bytesPerPixel);
            memcpy(srcData, trgData, bytesPerPixel);
            memcpy(trgData, save, bytesPerPixel);
        }
    }

    // Convert the texture data to a 32-bit floating-point format.
    mImage = new1<Float4>(mNumTexels);
    Color::FromFunction[mFormat](mNumTexels, mTexels, mImage);

    // We need to map the floating-point channels to [0,1] for display.
    mRGBMin = FLT_MAX;
    mRGBMax = -FLT_MAX;
    mAMin = FLT_MAX;
    mAMax = -FLT_MAX;
    for (int i = 0; i < mNumTexels; ++i)
    {
        Float4 color = mImage[i];
        for (int j = 0; j < 3; ++j)
        {
            if (color[j] < mRGBMin)
            {
                mRGBMin = color[j];
            }

            if (color[j] > mRGBMax)
            {
                mRGBMax = color[j];
            }
        }

        if (color[3] < mAMin)
        {
            mAMin = color[3];
        }

        if (color[3] > mAMax)
        {
            mAMax = color[3];
        }
    }

    if (0.0f <= mRGBMin && mRGBMax <= 1.0f)
    {
        mRGBMin = 0.0f;
        mRGBMax = 1.0f;
        mInvRGBRange = 1.0f;
    }
    else if (mRGBMax > mRGBMin)
    {
        mInvRGBRange = 1.0f/(mRGBMax - mRGBMin);
    }
    else
    {
        mInvRGBRange = 0.0f;
    }

    if (0.0f <= mAMin && mAMax <= 1.0f)
    {
        mAMin = 0.0f;
        mAMax = 1.0f;
        mInvARange = 1.0f;
    }
    else if (mAMax > mAMin)
    {
        mInvARange = 1.0f/(mAMax - mAMin);
    }
    else
    {
        mInvARange = 0.0f;
    }

    CopySliceToScreen();
    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void WmtfViewer::OnTerminate ()
{
    delete1(mImage);

    WindowApplication2::OnTerminate();
}
//----------------------------------------------------------------------------
bool WmtfViewer::OnKeyDown (unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'a':
    case 'A':
        mAlphaActive = !mAlphaActive;
        CopySliceToScreen();
        if (mMouseDown)
        {
            ReadPixelValue(x, y);
        }
        OnDisplay();
        return true;
    }

    return WindowApplication2::OnKeyDown(key, x, y);
}
//----------------------------------------------------------------------------
bool WmtfViewer::OnMouseClick (int button, int state, int x, int y,
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
bool WmtfViewer::OnMotion (int, int x, int y, unsigned int)
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
void WmtfViewer::ScreenOverlay ()
{
    if (mMouseDown)
    {
        mRenderer->Draw(4, mHeight - 4, mTextColor, mPixelString);
    }
}
//----------------------------------------------------------------------------
void WmtfViewer::CopySliceToScreen ()
{
    ClearScreen();

    int x, y, i, j;
    float fRed, fGreen, fBlue, fAlpha, fGray;
    unsigned char red, green, blue, alpha, gray;

    Float4* slice = &mImage[mSliceIndex];

    if (mAlphaActive)
    {
        if (mInvARange > 0.0f)
        {
            for (y = 0; y < mYDim; ++y)
            {
                for (x = 0; x < mXDim; ++x)
                {
                    i = x + mXDim*y;
                    fAlpha = (slice[i][3] - mAMin)*mInvARange;
                    alpha = (unsigned char)(255.0f*fAlpha);  

                    j = x + mScreenWidth*y;
                    mScreen[j].b = alpha;
                    mScreen[j].g = alpha;
                    mScreen[j].r = alpha;
                }
            }
        }
        else
        {
            fGray = 255.0f*slice[0][3];
            if (fGray < 0.0f)
            {
                fGray = 0.0f;
            }
            else if (fGray > 255.0f)
            {
                fGray = 255.0f;
            }
            gray = (unsigned char)fGray;

            for (y = 0; y < mYDim; ++y)
            {
                for (x = 0; x < mXDim; ++x)
                {
                    j = x + mScreenWidth*y;
                    mScreen[j].b = gray;
                    mScreen[j].g = gray;
                    mScreen[j].r = gray;
                }
            }
        }
    }
    else
    {
        if (mInvRGBRange > 0.0f)
        {
            for (y = 0; y < mYDim; ++y)
            {
                for (x = 0; x < mXDim; ++x)
                {
                    i = x + mXDim*y;
                    fRed = (slice[i][0] - mRGBMin)*mInvRGBRange;
                    fGreen = (slice[i][1] - mRGBMin)*mInvRGBRange;
                    fBlue = (slice[i][2] - mRGBMin)*mInvRGBRange;
                    red = (unsigned char)(255.0f*fRed);
                    green = (unsigned char)(255.0f*fGreen);
                    blue = (unsigned char)(255.0f*fBlue);

                    j = x + mScreenWidth*y;
                    mScreen[j].b = blue;
                    mScreen[j].g = green;
                    mScreen[j].r = red;
                }
            }
        }
        else
        {
            fGray = 255.0f*slice[0][0];
            if (fGray < 0.0f)
            {
                fGray = 0.0f;
            }
            else if (fGray > 255.0f)
            {
                fGray = 255.0f;
            }
            gray = (unsigned char)fGray;

            for (y = 0; y < mYDim; ++y)
            {
                for (x = 0; x < mXDim; ++x)
                {
                    j = x + mScreenWidth*y;
                    mScreen[j].b = gray;
                    mScreen[j].g = gray;
                    mScreen[j].r = gray;
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
void WmtfViewer::ReadPixelValue (int x, int y)
{
    if (0 <= x && x < mXDim && 0 <= y && y < mYDim)
    {
        (this->*mSFunction[mFormat])(x, y);
    }
    else
    {
        mPixelString[0] = ' ';
        mPixelString[1] = 0;
    }
}
//----------------------------------------------------------------------------
void WmtfViewer::SetStrR5G6B5 (int x, int y)
{
    if (mAlphaActive)
    {
        sprintf(mPixelString, "(%4d,%4d) no alpha",
            x, y);
    }
    else
    {
        const unsigned short* data = (const unsigned short*)mTexels;
        int i = x + mXDim*y;
        unsigned int value = data[i];
        unsigned int r = (value & 0x0000001Fu);
        unsigned int g = (value & 0x000007E0u) >> 5;
        unsigned int b = (value & 0x0000F800u) >> 11;
        sprintf(mPixelString, "(%4d,%4d) r = %2u , g = %2u , b = %2u",
            x, y, r, g, b);
    }
}
//----------------------------------------------------------------------------
void WmtfViewer::SetStrA1R5G5B5 (int x, int y)
{
    const unsigned short* data = (const unsigned short*)mTexels;
    int i = x + mXDim*y;
    unsigned int value = data[i];
    unsigned int a = (value & 0x00008000u) >> 15;

    if (mAlphaActive)
    {
        sprintf(mPixelString, "(%4d,%4d) a = %u",
            x, y, a);
    }
    else
    {
        unsigned int r = (value & 0x0000001Fu);
        unsigned int g = (value & 0x000003E0u) >> 5;
        unsigned int b = (value & 0x00007C00u) >> 10;
        sprintf(mPixelString,
            "(%4d,%4d) r = %2u , g = %2u , b = %2u , a = %u",
            x, y, r, g, b, a);
    }
}
//----------------------------------------------------------------------------
void WmtfViewer::SetStrA4R4G4B4 (int x, int y)
{
    const unsigned short* data = (const unsigned short*)mTexels;
    int i = x + mXDim*y;
    unsigned int value = data[i];
    unsigned int a = (value & 0x0000F000u) >> 12;

    if (mAlphaActive)
    {
        sprintf(mPixelString, "(%4d,%4d) a = %2u",
            x, y, a);
    }
    else
    {
        unsigned int r = (value & 0x0000000Fu);
        unsigned int g = (value & 0x000000F0u) >> 4;
        unsigned int b = (value & 0x00000F00u) >> 8;
        sprintf(mPixelString,
            "(%4d,%4d) r = %2u , g = %2u , b = %2u , a = %2u",
            x, y, r, g, b, a);
    }
}
//----------------------------------------------------------------------------
void WmtfViewer::SetStrA8 (int x, int y)
{
    if (mAlphaActive)
    {
        const unsigned char* data = (const unsigned char*)mTexels;
        int i = x + mXDim*y;
        unsigned int a = (unsigned int)data[i];
        sprintf(mPixelString, "(%4d,%4d) a = %3u",
            x, y, a);
    }
    else
    {
        sprintf(mPixelString, "(%4d,%4d) no color",
            x, y);
    }
}
//----------------------------------------------------------------------------
void WmtfViewer::SetStrL8 (int x, int y)
{
    if (mAlphaActive)
    {
        sprintf(mPixelString, "(%4d,%4d) no alpha",
            x, y);
    }
    else
    {
        const unsigned char* data = (const unsigned char*)mTexels;
        int i = x + mXDim*y;
        unsigned int intensity = (unsigned int)data[i];
        sprintf(mPixelString, "(%4d,%4d) i = %3u",
            x, y, intensity);
    }
}
//----------------------------------------------------------------------------
void WmtfViewer::SetStrA8L8 (int x, int y)
{
    const unsigned char* data = (const unsigned char*)mTexels;
    int i = 2*(x + mXDim*y);
    unsigned int a = data[i + 1];

    if (mAlphaActive)
    {
        sprintf(mPixelString, "(%4d,%4d) a = %3u",
            x, y, a);
    }
    else
    {
        unsigned int intensity = (unsigned int)data[i];
        sprintf(mPixelString, "(%4d,%4d) i = %3u , a = %3u",
            x, y, intensity, a);
    }
}
//----------------------------------------------------------------------------
void WmtfViewer::SetStrR8G8B8 (int x, int y)
{
    if (mAlphaActive)
    {
        sprintf(mPixelString, "(%4d,%4d) no alpha",
            x, y);
    }
    else
    {
        const unsigned char* data = (const unsigned char*)mTexels;
        int i = 3*(x + mXDim*y);
        unsigned int r = (unsigned int)data[i++];
        unsigned int g = (unsigned int)data[i++];
        unsigned int b = (unsigned int)data[i];
        sprintf(mPixelString, "(%4d,%4d) r = %3u , g = %3u , b = %3u",
            x, y, r, g, b);
    }
}
//----------------------------------------------------------------------------
void WmtfViewer::SetStrA8R8G8B8 (int x, int y)
{
    const unsigned char* data = (const unsigned char*)mTexels;
    int i = 4*(x + mXDim*y);

    if (mAlphaActive)
    {
        unsigned int a = (unsigned int)data[i + 3];
        sprintf(mPixelString, "(%4d,%4d) a = %3u",
            x, y, a);
    }
    else
    {
        unsigned int b = (unsigned int)data[i++];
        unsigned int g = (unsigned int)data[i++];
        unsigned int r = (unsigned int)data[i++];
        unsigned int a = (unsigned int)data[i];
        sprintf(mPixelString,
            "(%4d,%4d) r = %3u , g = %3u , b = %3u, a = %3u",
            x, y, r, g, b, a);
    }
}
//----------------------------------------------------------------------------
void WmtfViewer::SetStrA8B8G8R8 (int x, int y)
{
    const unsigned char* data = (const unsigned char*)mTexels;
    int i = 4*(x + mXDim*y);

    if (mAlphaActive)
    {
        unsigned int a = (unsigned int)data[i + 3];
        sprintf(mPixelString, "(%4d,%4d) a = %3u",
            x, y, a);
    }
    else
    {
        unsigned int r = (unsigned int)data[i++];
        unsigned int g = (unsigned int)data[i++];
        unsigned int b = (unsigned int)data[i++];
        unsigned int a = (unsigned int)data[i];
        sprintf(mPixelString,
            "(%4d,%4d) r = %3u , g = %3u , b = %3u, a = %3u",
            x, y, r, g, b, a);
    }
}
//----------------------------------------------------------------------------
void WmtfViewer::SetStrL16 (int x, int y)
{
    if (mAlphaActive)
    {
        sprintf(mPixelString, "(%4d,%4d) no alpha",
            x, y);
    }
    else
    {
        const unsigned short* data = (const unsigned short*)mTexels;
        int i = x + mXDim*y;
        unsigned int intensity = (unsigned int)data[i];
        sprintf(mPixelString, "(%4d,%4d) i = %u",
            x, y, intensity);
    }
}
//----------------------------------------------------------------------------
void WmtfViewer::SetStrG16R16 (int x, int y)
{
    if (mAlphaActive)
    {
        sprintf(mPixelString, "(%4d,%4d) no alpha",
            x, y);
    }
    else
    {
        const unsigned short* data = (const unsigned short*)mTexels;
        int i = 2*(x + mXDim*y);
        unsigned int r = (unsigned int)data[i++];
        unsigned int g = (unsigned int)data[i];
        sprintf(mPixelString, "(%4d,%4d) r = %u , g = %u",
            x, y, r, g);
    }
}
//----------------------------------------------------------------------------
void WmtfViewer::SetStrA16B16G16R16 (int x, int y)
{
    const unsigned short* data = (const unsigned short*)mTexels;
    int i = 4*(x + mXDim*y);
    unsigned int a = (unsigned int)data[i + 3];

    if (mAlphaActive)
    {
        sprintf(mPixelString, "(%4d,%4d) a = %u",
            x, y, a);
    }
    else
    {
        unsigned int r = (unsigned int)data[i++];
        unsigned int g = (unsigned int)data[i++];
        unsigned int b = (unsigned int)data[i];
        sprintf(mPixelString, "(%4d,%4d) r = %u , g = %u , b = %u , a = %u",
            x, y, r, g, b, a);
    }
}
//----------------------------------------------------------------------------
void WmtfViewer::SetStrR16F (int x, int y)
{
    if (mAlphaActive)
    {
        sprintf(mPixelString, "(%4d,%4d) no alpha",
            x, y);
    }
    else
    {
        const HalfFloat* data = (const HalfFloat*)mTexels;
        int i = x + mXDim*y;
        float r = ToFloat(data[i]);
        sprintf(mPixelString, "(%4d,%4d) r = %f",
            x, y, r);
    }
}
//----------------------------------------------------------------------------
void WmtfViewer::SetStrG16R16F (int x, int y)
{
    if (mAlphaActive)
    {
        sprintf(mPixelString, "(%4d,%4d) no alpha",
            x, y);
    }
    else
    {
        const HalfFloat* data = (const HalfFloat*)mTexels;
        int i = 2*(x + mXDim*y);
        float r = ToFloat(data[i++]);
        float g = ToFloat(data[i]);
        sprintf(mPixelString, "(%4d,%4d) r = %f , g = %f",
            x, y, r, g);
    }
}
//----------------------------------------------------------------------------
void WmtfViewer::SetStrA16B16G16R16F (int x, int y)
{
    const HalfFloat* data = (const HalfFloat*)mTexels;
    int i = 4*(x + mXDim*y);
    float a = ToFloat(data[i + 3]);

    if (mAlphaActive)
    {
        sprintf(mPixelString, "(%4d,%4d) a = %f",
            x, y, a);
    }
    else
    {
        float r = ToFloat(data[i++]);
        float g = ToFloat(data[i++]);
        float b = ToFloat(data[i]);
        sprintf(mPixelString, "(%4d,%4d) r = %f , g = %f , b = %f , a = %f",
            x, y, r, g, b, a);
    }
}
//----------------------------------------------------------------------------
void WmtfViewer::SetStrR32F (int x, int y)
{
    if (mAlphaActive)
    {
        sprintf(mPixelString, "(%4d,%4d) no alpha",
            x, y);
    }
    else
    {
        const float* data = (const float*)mTexels;
        int i = x + mXDim*y;
        float r = data[i];
        sprintf(mPixelString, "(%4d,%4d) r = %f",
            x, y, r);
    }
}
//----------------------------------------------------------------------------
void WmtfViewer::SetStrG32R32F (int x, int y)
{
    if (mAlphaActive)
    {
        sprintf(mPixelString, "(%4d,%4d) no alpha",
            x, y);
    }
    else
    {
        const Float2* data = (const Float2*)mTexels;
        int i = x + mXDim*y;
        float r = data[i][0];
        float g = data[i][1];
        sprintf(mPixelString, "(%4d,%4d) r = %f , g = %f",
            x, y, r, g);
    }
}
//----------------------------------------------------------------------------
void WmtfViewer::SetStrA32B32G32R32F (int x, int y)
{
    const Float4* data = (const Float4*)mTexels;
    int i = x + mXDim*y;
    float a = data[i][3];

    if (mAlphaActive)
    {
        sprintf(mPixelString, "(%4d,%4d) a = %f",
            x, y, a);
    }
    else
    {
        float r = data[i][0];
        float g = data[i][1];
        float b = data[i][2];
        sprintf(mPixelString, "(%4d,%4d) r = %f , g = %f , b = %f , a = %f",
            x, y, r, g, b, a);
    }
}
//----------------------------------------------------------------------------
