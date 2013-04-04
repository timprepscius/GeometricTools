// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

//----------------------------------------------------------------------------
inline int Image::GetPixelSize () const
{
    return mPixelSize;
}
//----------------------------------------------------------------------------
inline int Image::GetNumDimensions () const
{
    return mNumDimensions;
}
//----------------------------------------------------------------------------
inline const int* Image::GetDimensions () const
{
    return mDimensions;
}
//----------------------------------------------------------------------------
inline const int* Image::GetOffsets () const
{
    return mOffsets;
}
//----------------------------------------------------------------------------
inline int Image::GetNumPixels () const
{
    return mNumPixels;
}
//----------------------------------------------------------------------------
inline const char* Image::GetRawPixels () const
{
    return mRawPixels;
}
//----------------------------------------------------------------------------
inline bool Image::IsNullImage () const
{
    // There are many tests for a null image, all equivalent.
    return mRawPixels == 0;
}
//----------------------------------------------------------------------------
