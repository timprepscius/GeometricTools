// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

//----------------------------------------------------------------------------
template <typename PixelType>
Image2<PixelType>::Image2 ()
    :
    Image(sizeof(PixelType), 0),
    mPixels(0)
{
}
//----------------------------------------------------------------------------
template <typename PixelType>
Image2<PixelType>::~Image2 ()
{
    DeallocatePointers();
}
//----------------------------------------------------------------------------
template <typename PixelType>
Image2<PixelType>::Image2 (const Image2& image)
    :
    Image(image),
    mPixels(0)
{
    AllocatePointers();
}
//----------------------------------------------------------------------------
template <typename PixelType>
Image2<PixelType>::Image2 (int dimension0, int dimension1)
    :
    Image(sizeof(PixelType), 2,  dimension0, dimension1),
    mPixels(0)
{
    AllocatePointers();
}
//----------------------------------------------------------------------------
template <typename PixelType>
Image2<PixelType>& Image2<PixelType>::operator= (const Image2& image)
{
    bool compatible = Copy(image);
    if (!compatible)
    {
        AllocatePointers();
    }
    return *this;
}
//----------------------------------------------------------------------------
template <typename PixelType>
inline int Image2<PixelType>::GetIndex (int x, int y) const
{
#ifdef IMAGE_RANGE_CHECK
    if (0 <= x && x < mDimensions[0] && 0 <= y && y < mDimensions[1])
    {
        return x + mDimensions[0]*y;
    }
    assertion(false, "Range check failure.\n");
    return 0;
#else
    return x + mDimensions[0]*y;
#endif
}
//----------------------------------------------------------------------------
template <typename PixelType>
inline int Image2<PixelType>::GetIndex (const Tuple<2,int>& coord) const
{
#ifdef IMAGE_RANGE_CHECK
    if (0 <= coord[0] && coord[0] < mDimensions[0]
    &&  0 <= coord[1] && coord[1] < mDimensions[1])
    {
        return coord[0] + mDimensions[0]*coord[1];
    }
    assertion(false, "Range check failure.\n");
    return 0;
#else
    return coord[0] + mDimensions[0]*coord[1];
#endif
}
//----------------------------------------------------------------------------
template <typename PixelType>
inline void Image2<PixelType>::GetCoordinates (int index, int& x, int& y)
    const
{
#ifdef IMAGE_RANGE_CHECK
    if (0 <= index && index < mNumPixels)
    {
        x = index % mDimensions[0];
        y = index / mDimensions[0];
    }
    assertion(false, "Range check failure.\n");
    x = 0;
    y = 0;
#else
    x = index % mDimensions[0];
    y = index / mDimensions[0];
#endif
}
//----------------------------------------------------------------------------
template <typename PixelType>
inline Tuple<2,int> Image2<PixelType>::GetCoordinates (int index) const
{
    Tuple<2,int> coord;
#ifdef IMAGE_RANGE_CHECK
    if (0 <= index && index < mNumPixels)
    {
        coord[0] = index % mDimensions[0];
        coord[1] = index / mDimensions[0];
    }
    else
    {
        assertion(false, "Range check failure.\n");
        coord[0] = 0;
        coord[1] = 0;
    }
#else
    coord[0] = index % mDimensions[0];
    coord[1] = index / mDimensions[0];
#endif
    return coord;
}
//----------------------------------------------------------------------------
template <typename PixelType>
inline PixelType* Image2<PixelType>::GetPixels1D () const
{
    if (mPixels)
    {
        return mPixels[0];
    }
    assertion(false, "Pixels do not exist.\n");
    return 0;
}
//----------------------------------------------------------------------------
template <typename PixelType>
inline PixelType& Image2<PixelType>::operator[] (int i)
{
#ifdef IMAGE_RANGE_CHECK
    if (mPixels != 0 && 0 <= i && i < mNumPixels)
    {
        return mPixels[0][i];
    }
    assertion(false, "Range check failure.\n");
    return mInvalidPixel;
#else
    return mPixels[0][i];
#endif
}
//----------------------------------------------------------------------------
template <typename PixelType>
inline const PixelType& Image2<PixelType>::operator[] (int i) const
{
#ifdef IMAGE_RANGE_CHECK
    if (mPixels != 0 && 0 <= i && i < mNumPixels)
    {
        return mPixels[0][i];
    }
    assertion(false, "Range check failure.\n");
    return mInvalidPixel;
#else
    return mPixels[0][i];
#endif
}
//----------------------------------------------------------------------------
template <typename PixelType>
PixelType& Image2<PixelType>::Get (int i)
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
        return mPixels[0][i];
    }
    return mInvalidPixel;
}
//----------------------------------------------------------------------------
template <typename PixelType>
const PixelType& Image2<PixelType>::Get (int i) const
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
        return mPixels[0][i];
    }
    return mInvalidPixel;
}
//----------------------------------------------------------------------------
template <typename PixelType>
inline PixelType** Image2<PixelType>::GetPixels2D () const
{
    assertion(mPixels != 0, "Pixels do not exist.\n");
    return mPixels;
}
//----------------------------------------------------------------------------
template <typename PixelType>
inline PixelType& Image2<PixelType>::operator() (int x, int y)
{
#ifdef IMAGE_RANGE_CHECK
    if (mPixels != 0
    &&  0 <= x && x < mDimensions[0]
    &&  0 <= y && y < mDimensions[1])
    {
        return mPixels[y][x];
    }
    assertion(false, "Range check failure.\n");
    return mInvalidPixel;
#else
    return mPixels[y][x];
#endif
}
//----------------------------------------------------------------------------
template <typename PixelType>
inline const PixelType& Image2<PixelType>::operator() (int x, int y) const
{
#ifdef IMAGE_RANGE_CHECK
    if (mPixels != 0
    &&  0 <= x && x < mDimensions[0]
    &&  0 <= y && y < mDimensions[1])
    {
        return mPixels[y][x];
    }
    assertion(false, "Range check failure.\n");
    return mInvalidPixel;
#else
    return mPixels[y][x];
#endif
}
//----------------------------------------------------------------------------
template <typename PixelType>
inline PixelType& Image2<PixelType>::operator() (const Tuple<2,int>& coord)
{
#ifdef IMAGE_RANGE_CHECK
    if (mPixels != 0
    &&  0 <= coord[0] && coord[0] < mDimensions[0]
    &&  0 <= coord[1] && coord[1] < mDimensions[1])
    {
        return mPixels[coord[1]][coord[0]];
    }
    assertion(false, "Range check failure.\n");
    return mInvalidPixel;
#else
    return mPixels[coord[1]][coord[0]];
#endif
}
//----------------------------------------------------------------------------
template <typename PixelType>
inline const PixelType& Image2<PixelType>::operator() (
    const Tuple<2,int>& coord) const
{
#ifdef IMAGE_RANGE_CHECK
    if (mPixels != 0
    &&  0 <= coord[0] && coord[0] < mDimensions[0]
    &&  0 <= coord[1] && coord[1] < mDimensions[1])
    {
        return mPixels[coord[1]][coord[0]];
    }
    assertion(false, "Range check failure.\n");
    return mInvalidPixel;
#else
    return mPixels[coord[1]][coord[0]];
#endif
}
//----------------------------------------------------------------------------
template <typename PixelType>
PixelType& Image2<PixelType>::Get (int x, int y)
{
    if (mPixels)
    {
        // Clamp to valid (x,y).
        if (x < 0)
        {
            x = 0;
        }
        else if (x >= mDimensions[0])
        {
            x = mDimensions[0] - 1;
        }

        if (y < 0)
        {
            y = 0;
        }
        else if (y >= mDimensions[1])
        {
            y = mDimensions[1] - 1;
        }

        return mPixels[y][x];
    }
    return mInvalidPixel;
}
//----------------------------------------------------------------------------
template <typename PixelType>
const PixelType& Image2<PixelType>::Get (int x, int y) const
{
    if (mPixels)
    {
        // Clamp to valid (x,y).
        if (x < 0)
        {
            x = 0;
        }
        else if (x >= mDimensions[0])
        {
            x = mDimensions[0] - 1;
        }

        if (y < 0)
        {
            y = 0;
        }
        else if (y >= mDimensions[1])
        {
            y = mDimensions[1] - 1;
        }

        return mPixels[y][x];
    }
    return mInvalidPixel;
}
//----------------------------------------------------------------------------
template <typename PixelType>
PixelType& Image2<PixelType>::Get (Tuple<2,int> coord)
{
    if (mPixels)
    {
        // Clamp to valid (x,y).
        for (int i = 0; i < 2; ++i)
        {
            if (coord[i] < 0)
            {
                coord[i] = 0;
            }
            else if (coord[i] >= mDimensions[i])
            {
                coord[i] = mDimensions[i] - 1;
            }
        }

        return mPixels[coord[1]][coord[0]];
    }
    return mInvalidPixel;
}
//----------------------------------------------------------------------------
template <typename PixelType>
const PixelType& Image2<PixelType>::Get (Tuple<2,int> coord) const
{
    if (mPixels)
    {
        // Clamp to valid (x,y).
        for (int i = 0; i < 2; ++i)
        {
            if (coord[i] < 0)
            {
                coord[i] = 0;
            }
            else if (coord[i] >= mDimensions[i])
            {
                coord[i] = mDimensions[i] - 1;
            }
        }

        return mPixels[coord[1]][coord[0]];
    }
    return mInvalidPixel;
}
//----------------------------------------------------------------------------
template <typename PixelType>
bool Image2<PixelType>::Resize (int dimension0, int dimension1)
{
    if (Image::Resize(2, dimension0, dimension1))
    {
        AllocatePointers();
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
template <typename PixelType>
void Image2<PixelType>::SetAllPixels (const PixelType& value)
{
    if (mPixels)
    {
        for (int i = 0; i < mNumPixels; ++i)
        {
            mPixels[0][i] = value;
        }
    }
}
//----------------------------------------------------------------------------
template <typename PixelType>
void Image2<PixelType>::AllocatePointers ()
{
    if (mPixels)
    {
        delete1(mPixels);
    }

    mPixels = new1<PixelType*>(mDimensions[1]);
    mPixels[0] = (PixelType*)mRawPixels;
    for (int i1 = 1; i1 < mDimensions[1]; ++i1)
    {
        int j0 = mDimensions[0]*i1;  // = mDimensions[0]*(i1 + j1), j1 = 0
        mPixels[i1] = &mPixels[0][j0];
    }
}
//----------------------------------------------------------------------------
template <typename PixelType>
void Image2<PixelType>::DeallocatePointers ()
{
    delete1(mPixels);
    mPixels = 0;
}
//----------------------------------------------------------------------------
