// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

#include "Image.h"
#include "Wm5Memory.h"

//----------------------------------------------------------------------------
Image::~Image ()
{
    delete1(mDimensions);
    delete1(mOffsets);
    delete1(mRawPixels);
}
//----------------------------------------------------------------------------
Image::Image ()
    :
    mPixelSize(0),
    mNumDimensions(0),
    mDimensions(0),
    mOffsets(0),
    mNumPixels(0),
    mRawPixels(0)
{
}
//----------------------------------------------------------------------------
Image::Image (const Image& image)
    :
    mPixelSize(0),
    mNumDimensions(0),
    mDimensions(0),
    mOffsets(0),
    mNumPixels(0),
    mRawPixels(0)
{
    *this = image;
}
//----------------------------------------------------------------------------
Image::Image (int pixelSize, int numDimensions, ...)
    :
    mPixelSize(pixelSize),
    mNumDimensions(numDimensions),
    mDimensions(0),
    mOffsets(0),
    mNumPixels(0),
    mRawPixels(0)
{
    if (mNumDimensions > 0)
    {
        mDimensions = new1<int>(mNumDimensions);
        mOffsets = new1<int>(mNumDimensions);

        va_list arguments;
        va_start(arguments, numDimensions);
        mNumPixels = 1;
        int i;
        for (i = 0; i < mNumDimensions; ++i)
        {
            mDimensions[i] = va_arg(arguments, int);
            mNumPixels *= mDimensions[i];
        }
        va_end(arguments);

        mOffsets[0] = 1;
        for (i = 1; i < mNumDimensions; ++i)
        {
            mOffsets[i] = mDimensions[i - 1]*mOffsets[i - 1];
        }

        mRawPixels = new1<char>(mNumPixels*mPixelSize);
        return;
    }
}
//----------------------------------------------------------------------------
Image& Image::operator= (const Image&)
{
    // Following the Rule of Three.  The 'bool Copy (const Image&)' function
    // must be used instead so that the IsCompatible(...) bool may be used
    // by the caller.
    assertion(false, "This function should not be called.\n");
    return *this;
}
//----------------------------------------------------------------------------
bool Image::Copy (const Image& image)
{
    bool compatible = IsCompatible(image);
    if (!compatible)
    {
        CreateNullImage();

        mPixelSize = image.mPixelSize;
        mNumDimensions = image.mNumDimensions;
        mNumPixels = image.mNumPixels;

        if (mNumDimensions > 0)
        {
            mDimensions = new1<int>(mNumDimensions);
            mOffsets = new1<int>(mNumDimensions);
            for (int i = 0; i < mNumDimensions; ++i)
            {
                mDimensions[i] = image.mDimensions[i];
                mOffsets[i] = image.mOffsets[i];
            }

            mRawPixels = new1<char>(mNumPixels*mPixelSize);
        }
        else
        {
            mDimensions = 0;
            mOffsets = 0;
            mRawPixels = 0;
        }
    }

    if (mRawPixels)
    {
        memcpy(mRawPixels, image.mRawPixels, mNumPixels*mPixelSize);
    }

    return compatible;
}
//----------------------------------------------------------------------------
int Image::GetDimension (int i) const
{
#ifdef IMAGE_RANGE_CHECK
    if (0 <= i && i < mNumDimensions)
    {
        return mDimensions[i];
    }
    assertion(false, "Range check failure.\n");
    return 0;
#else
    return mDimensions[i];
#endif
}
//----------------------------------------------------------------------------
int Image::GetOffset (int i) const
{
#ifdef IMAGE_RANGE_CHECK
    if (0 <= i && i < mNumDimensions)
    {
        return mOffsets[i];
    }
    assertion(false, "Range check failure.\n");
    return 0;
#else
    return mOffsets[i];
#endif
}
//----------------------------------------------------------------------------
int Image::GetIndex (const int* coord) const
{
    // assert:  coord is array of mNumDimensions elements
    int index = coord[0];
    for (int i = 1; i < mNumDimensions; ++i)
    {
        index += mOffsets[i]*coord[i];
    }
    return index;
}
//----------------------------------------------------------------------------
void Image::GetCoordinates (int index, int* coord) const
{
    // assert:  coord is array of mNumDimensions elements
    for (int i = 0; i < mNumDimensions; ++i)
    {
        coord[i] = index % mDimensions[i];
        index /= mDimensions[i];
    }
}
//----------------------------------------------------------------------------
void Image::ClearPixels ()
{
    if (mRawPixels)
    {
        memset(mRawPixels, 0, mNumPixels*mPixelSize*sizeof(char));
    }
}
//----------------------------------------------------------------------------
void Image::CreateNullImage ()
{
    delete1(mDimensions);
    delete1(mOffsets);
    delete1(mRawPixels);
    mPixelSize = 0;
    mNumDimensions = 0;
    mDimensions = 0;
    mOffsets = 0;
    mNumPixels = 0;
    mRawPixels = 0;
}
//----------------------------------------------------------------------------
bool Image::Resize (int numDimensions, ...)
{
    if (numDimensions < 0)
    {
        assertion(false, "Invalid number of dimensions.\n");
        numDimensions = 0;
    }
    else if (numDimensions > 0 && numDimensions == mNumDimensions)
    {
        // Test for compatibility.  If they are, no resizing is necessary.
        va_list arguments;
        va_start(arguments, numDimensions);
        int i;
        for (i = 0; i < mNumDimensions; ++i)
        {
            int inputDimension = va_arg(arguments, int);
            if (mDimensions[i] != inputDimension)
            {
                break;
            }
        }
        va_end(arguments);
        if (i == mNumDimensions)
        {
            // The images are compatible.
            return false;
        }
    }

    int savePixelSize = mPixelSize;
    CreateNullImage();
    mPixelSize = savePixelSize;
    mNumDimensions = numDimensions;

    if (mNumDimensions > 0)
    {
        mDimensions = new1<int>(mNumDimensions);
        mOffsets = new1<int>(mNumDimensions);

        va_list arguments;
        va_start(arguments, numDimensions);
        mNumPixels = 1;
        int i;
        for (i = 0; i < mNumDimensions; ++i)
        {
            mDimensions[i] = va_arg(arguments, int);
            mNumPixels *= mDimensions[i];
        }
        va_end(arguments);

        mOffsets[0] = 1;
        for (i = 1; i < mNumDimensions; ++i)
        {
            mOffsets[i] = mDimensions[i - 1]*mOffsets[i - 1];
        }

        mRawPixels = new1<char>(mNumPixels*mPixelSize);
    }

    return true;
}
//----------------------------------------------------------------------------
bool Image::IsCompatible (const Image& image) const
{
    if (mNumDimensions != image.mNumDimensions)
    {
        return false;
    }

    for (int i = 0; i < mNumDimensions; ++i)
    {
        if (mDimensions[i] != image.mDimensions[i])
        {
            return false;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
