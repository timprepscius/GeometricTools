// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

//----------------------------------------------------------------------------
template <typename PixelType>
Image1<PixelType>::Image1 ()
    :
    Image(sizeof(PixelType), 0),
    mPixels(0)
{
}
//----------------------------------------------------------------------------
template <typename PixelType>
Image1<PixelType>::~Image1 ()
{
}
//----------------------------------------------------------------------------
template <typename PixelType>
Image1<PixelType>::Image1 (const Image1& image)
    :
    Image(image),
    mPixels((PixelType*)mRawPixels)
{
}
//----------------------------------------------------------------------------
template <typename PixelType>
Image1<PixelType>::Image1 (int dimension)
    :
    Image(sizeof(PixelType), 1, dimension),
    mPixels((PixelType*)mRawPixels)
{
}
//----------------------------------------------------------------------------
template <typename PixelType>
Image1<PixelType>& Image1<PixelType>::operator= (const Image1& image)
{
    bool compatible = Copy(image);
    if (!compatible)
    {
        mPixels = (PixelType*)mRawPixels;
    }
    return *this;
}
//----------------------------------------------------------------------------
template <typename PixelType>
inline PixelType* Image1<PixelType>::GetPixels1D () const
{
    return mPixels;
}
//----------------------------------------------------------------------------
template <typename PixelType>
inline PixelType& Image1<PixelType>::operator[] (int i)
{
#ifdef IMAGE_RANGE_CHECK
    if (mPixels && 0 <= i && i < mNumPixels)
    {
        return mPixels[i];
    }
    assertion(false, "Range check failure.\n");
    return mInvalidPixel;
#else
    return mPixels[i];
#endif
}
//----------------------------------------------------------------------------
template <typename PixelType>
inline const PixelType& Image1<PixelType>::operator[] (int i) const
{
#ifdef IMAGE_RANGE_CHECK
    if (mPixels && 0 <= i && i < mNumPixels)
    {
        return mPixels[i];
    }
    assertion(false, "Range check failure.\n");
    return mInvalidPixel;
#else
    return mPixels[i];
#endif
}
//----------------------------------------------------------------------------
template <typename PixelType>
PixelType& Image1<PixelType>::Get (int i)
{
    if (mPixels)
    {
        if (i < 0)
        {
            i = 0;
        }
        else if (i >= mNumPixels)
        {
            i = mNumPixels - 1;
        }
        return mPixels[i];
    }
    return mInvalidPixel;
}
//----------------------------------------------------------------------------
template <typename PixelType>
const PixelType& Image1<PixelType>::Get (int i) const
{
    if (mPixels)
    {
        if (i < 0)
        {
            i = 0;
        }
        else if (i >= mNumPixels)
        {
            i = mNumPixels - 1;
        }
        return mPixels[i];
    }
    return mInvalidPixel;
}
//----------------------------------------------------------------------------
template <typename PixelType>
inline PixelType& Image1<PixelType>::operator() (int x)
{
#ifdef IMAGE_RANGE_CHECK
    if (mPixels != 0 && 0 <= x && x < mDimensions[0])
    {
        return mPixels[x];
    }
    assertion(false, "Range check failure.\n");
    return mInvalidPixel;
#else
    return mPixels[x];
#endif
}
//----------------------------------------------------------------------------
template <typename PixelType>
inline const PixelType& Image1<PixelType>::operator() (int x) const
{
#ifdef IMAGE_RANGE_CHECK
    if (mPixels != 0 && 0 <= x && x < mDimensions[0])
    {
        return mPixels[x];
    }
    assertion(false, "Range check failure.\n");
    return mInvalidPixel;
#else
    return mPixels[x];
#endif
}
//----------------------------------------------------------------------------
template <typename PixelType>
inline PixelType& Image1<PixelType>::operator() (const Tuple<1,int>& coord)
{
#ifdef IMAGE_RANGE_CHECK
    if (mPixels != 0 && 0 <= coord[0] && coord[0] < mDimensions[0])
    {
        return mPixels[coord[0]];
    }
    assertion(false, "Range check failure.\n");
    return mInvalidPixel;
#else
    return mPixels[coord[0]];
#endif
}
//----------------------------------------------------------------------------
template <typename PixelType>
inline const PixelType& Image1<PixelType>::operator() (
    const Tuple<1,int>& coord) const
{
#ifdef IMAGE_RANGE_CHECK
    if (mPixels != 0 && 0 <= coord[0] && coord[0] < mDimensions[0])
    {
        return mPixels[coord[0]];
    }
    assertion(false, "Range check failure.\n");
    return mInvalidPixel;
#else
    return mPixels[coord[0]];
#endif
}
//----------------------------------------------------------------------------
template <typename PixelType>
PixelType& Image1<PixelType>::Get (Tuple<1,int> coord)
{
    if (mPixels)
    {
        // Clamp to valid (x).
        if (coord[0] < 0)
        {
            coord[0] = 0;
        }
        else if (coord[0] >= mDimensions[0])
        {
            coord[0] = mDimensions[0] - 1;
        }

        return mPixels[coord[0]];
    }
    return mInvalidPixel;
}
//----------------------------------------------------------------------------
template <typename PixelType>
const PixelType& Image1<PixelType>::Get (Tuple<1,int> coord) const
{
    if (mPixels)
    {
        // Clamp to valid (x).
        if (coord[0] < 0)
        {
            coord[0] = 0;
        }
        else if (coord[0] >= mDimensions[0])
        {
            coord[0] = mDimensions[0] - 1;
        }

        return mPixels[coord[0]];
    }
    return mInvalidPixel;
}
//----------------------------------------------------------------------------
template <typename PixelType>
bool Image1<PixelType>::Resize (int dimension)
{
    if (Image::Resize(1, dimension))
    {
        mPixels = (PixelType*)mRawPixels;
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
template <typename PixelType>
void Image1<PixelType>::SetAllPixels (const PixelType& value)
{
    if (mPixels)
    {
        for (int i = 0; i < mNumPixels; ++i)
        {
            mPixels[i] = value;
        }
    }
}
//----------------------------------------------------------------------------
