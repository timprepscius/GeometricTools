// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

//----------------------------------------------------------------------------
template <typename PixelType>
Image3<PixelType>::Image3 ()
    :
    Image(sizeof(PixelType), 0),
    mPixels(0)
{
}
//----------------------------------------------------------------------------
template <typename PixelType>
Image3<PixelType>::~Image3 ()
{
    DeallocatePointers();
}
//----------------------------------------------------------------------------
template <typename PixelType>
Image3<PixelType>::Image3 (const Image3& image)
    :
    Image(image),
    mPixels(0)
{
    AllocatePointers();
}
//----------------------------------------------------------------------------
template <typename PixelType>
Image3<PixelType>::Image3 (int dimension0, int dimension1, int dimension2)
    :
    Image(sizeof(PixelType), 3, dimension0, dimension1, dimension2),
    mPixels(0)
{
    AllocatePointers();
}
//----------------------------------------------------------------------------
template <typename PixelType>
Image3<PixelType>& Image3<PixelType>::operator= (const Image3& image)
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
inline int Image3<PixelType>::GetIndex (int x, int y, int z) const
{
#ifdef IMAGE_RANGE_CHECK
    if (0 <= x && x < mDimensions[0]
    &&  0 <= y && y < mDimensions[1]
    &&  0 <= z && z < mDimensions[2])
    {
        return x + mDimensions[0]*(y + mDimensions[1]*z);
    }
    assertion(false, "Range check failure.\n");
    return 0;
#else
    return x + mDimensions[0]*(y + mDimensions[1]*z);
#endif
}
//----------------------------------------------------------------------------
template <typename PixelType>
inline int Image3<PixelType>::GetIndex (const Tuple<3,int>& coord) const
{
#ifdef IMAGE_RANGE_CHECK
    if (0 <= coord[0] && coord[0] < mDimensions[0]
    &&  0 <= coord[1] && coord[1] < mDimensions[1]
    &&  0 <= coord[2] && coord[2] < mDimensions[2])
    {
        return coord[0] + mDimensions[0]*(coord[1] + mDimensions[1]*coord[2]);
    }
    assertion(false, "Range check failure.\n");
    return 0;
#else
    return coord[0] + mDimensions[0]*(coord[1] + mDimensions[1]*coord[2]);
#endif
}
//----------------------------------------------------------------------------
template <typename PixelType>
inline void Image3<PixelType>::GetCoordinates (int index, int& x, int& y,
    int& z) const
{
#ifdef IMAGE_RANGE_CHECK
    if (0 <= index && index < mNumPixels)
    {
        x = index % mDimensions[0];
        index /= mDimensions[0];
        y = index % mDimensions[1];
        z = index / mDimensions[1];
    }
    assertion(false, "Range check failure.\n");
    x = 0;
    y = 0;
    z = 0;
#else
    x = index % mDimensions[0];
    index /= mDimensions[0];
    y = index % mDimensions[1];
    z = index / mDimensions[1];
#endif
}
//----------------------------------------------------------------------------
template <typename PixelType>
inline Tuple<3,int> Image3<PixelType>::GetCoordinates (int index) const
{
    Tuple<3,int> coord;
#ifdef IMAGE_RANGE_CHECK
    if (0 <= index && index < mNumPixels)
    {
        coord[0] = index % mDimensions[0];
        index /= mDimensions[0];
        coord[1] = index % mDimensions[1];
        coord[2] = index / mDimensions[1];
    }
    else
    {
        assertion(false, "Range check failure.\n");
        coord[0] = 0;
        coord[1] = 0;
        coord[2] = 0;
    }
#else
    coord[0] = index % mDimensions[0];
    index /= mDimensions[0];
    coord[1] = index % mDimensions[1];
    coord[2] = index / mDimensions[1];
#endif
    return coord;
}
//----------------------------------------------------------------------------
template <typename PixelType>
inline PixelType* Image3<PixelType>::GetPixels1D () const
{
    if (mPixels)
    {
        return mPixels[0][0];
    }
    assertion(false, "Pixels do not exist.\n");
    return 0;
}
//----------------------------------------------------------------------------
template <typename PixelType>
inline PixelType& Image3<PixelType>::operator[] (int i)
{
#ifdef IMAGE_RANGE_CHECK
    if (mPixels != 0 && 0 <= i && i < mNumPixels)
    {
        return mPixels[0][0][i];
    }
    assertion(false, "Range check failure.\n");
    return mInvalidPixel;
#else
    return mPixels[0][0][i];
#endif
}
//----------------------------------------------------------------------------
template <typename PixelType>
inline const PixelType& Image3<PixelType>::operator[] (int i) const
{
#ifdef IMAGE_RANGE_CHECK
    if (mPixels != 0 && 0 <= i && i < mNumPixels)
    {
        return mPixels[0][0][i];
    }
    assertion(false, "Range check failure.\n");
    return mInvalidPixel;
#else
    return mPixels[0][0][i];
#endif
}
//----------------------------------------------------------------------------
template <typename PixelType>
PixelType& Image3<PixelType>::Get (int i)
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
        return mPixels[0][0][i];
    }
    return mInvalidPixel;
}
//----------------------------------------------------------------------------
template <typename PixelType>
const PixelType& Image3<PixelType>::Get (int i) const
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
        return mPixels[0][0][i];
    }
    return mInvalidPixel;
}
//----------------------------------------------------------------------------
template <typename PixelType>
inline PixelType*** Image3<PixelType>::GetPixels3D () const
{
    assertion(mPixels != 0, "Pixels do not exist.\n");
    return mPixels;
}
//----------------------------------------------------------------------------
template <typename PixelType>
inline PixelType& Image3<PixelType>::operator() (int x, int y, int z)
{
#ifdef IMAGE_RANGE_CHECK
    if (mPixels != 0
    &&  0 <= x && x < mDimensions[0]
    &&  0 <= y && y < mDimensions[1]
    &&  0 <= z && z < mDimensions[2])
    {
        return mPixels[z][y][x];
    }
    assertion(false, "Range check failure.\n");
    return mInvalidPixel;
#else
    return mPixels[z][y][x];
#endif
}
//----------------------------------------------------------------------------
template <typename PixelType>
inline const PixelType& Image3<PixelType>::operator() (int x, int y, int z)
    const
{
#ifdef IMAGE_RANGE_CHECK
    if (mPixels != 0
    &&  0 <= x && x < mDimensions[0]
    &&  0 <= y && y < mDimensions[1]
    &&  0 <= z && z < mDimensions[2])
    {
        return mPixels[z][y][x];
    }
    assertion(false, "Range check failure.\n");
    return mInvalidPixel;
#else
    return mPixels[z][y][x];
#endif
}
//----------------------------------------------------------------------------
template <typename PixelType>
inline PixelType& Image3<PixelType>::operator() (const Tuple<3,int>& coord)
{
#ifdef IMAGE_RANGE_CHECK
    if (mPixels != 0
    &&  0 <= coord[0] && coord[0] < mDimensions[0]
    &&  0 <= coord[1] && coord[1] < mDimensions[1]
    &&  0 <= coord[2] && coord[2] < mDimensions[2])
    {
        return mPixels[coord[2]][coord[1]][coord[0]];
    }
    assertion(false, "Range check failure.\n");
    return mInvalidPixel;
#else
    return mPixels[coord[2]][coord[1]][coord[0]];
#endif
}
//----------------------------------------------------------------------------
template <typename PixelType>
inline const PixelType& Image3<PixelType>::operator() (
    const Tuple<3,int>& coord) const
{
#ifdef IMAGE_RANGE_CHECK
    if (mPixels != 0
    &&  0 <= coord[0] && coord[0] < mDimensions[0]
    &&  0 <= coord[1] && coord[1] < mDimensions[1]
    &&  0 <= coord[2] && coord[2] < mDimensions[2])
    {
        return mPixels[coord[2]][coord[1]][coord[0]];
    }
    assertion(false, "Range check failure.\n");
    return mInvalidPixel;
#else
    return mPixels[coord[2]][coord[1]][coord[0]];
#endif
}
//----------------------------------------------------------------------------
template <typename PixelType>
PixelType& Image3<PixelType>::Get (int x, int y, int z)
{
    if (mPixels)
    {
        // Clamp to valid (x,y,z).
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

        if (z < 0)
        {
            z = 0;
        }
        else if (z >= mDimensions[2])
        {
            z = mDimensions[2] - 1;
        }

        return mPixels[z][y][x];
    }
    return mInvalidPixel;
}
//----------------------------------------------------------------------------
template <typename PixelType>
const PixelType& Image3<PixelType>::Get (int x, int y, int z) const
{
    if (mPixels)
    {
        // Clamp to valid (x,y,z).
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

        if (z < 0)
        {
            z = 0;
        }
        else if (z >= mDimensions[2])
        {
            z = mDimensions[2] - 1;
        }

        return mPixels[z][y][x];
    }
    return mInvalidPixel;
}
//----------------------------------------------------------------------------
template <typename PixelType>
PixelType& Image3<PixelType>::Get (Tuple<3,int> coord)
{
    if (mPixels)
    {
        // Clamp to valid (x,y,z).
        for (int i = 0; i < 3; ++i)
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

        return mPixels[coord[2]][coord[1]][coord[0]];
    }
    return mInvalidPixel;
}
//----------------------------------------------------------------------------
template <typename PixelType>
const PixelType& Image3<PixelType>::Get (Tuple<3,int> coord) const
{
    if (mPixels)
    {
        // Clamp to valid (x,y,z).
        for (int i = 0; i < 3; ++i)
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

        return mPixels[coord[2]][coord[1]][coord[0]];
    }
    return mInvalidPixel;
}
//----------------------------------------------------------------------------
template <typename PixelType>
bool Image3<PixelType>::Resize (int dimension0, int dimension1,
    int dimension2)
{
    if (Image::Resize(3, dimension0, dimension1, dimension2))
    {
        AllocatePointers();
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
template <typename PixelType>
void Image3<PixelType>::SetAllPixels (const PixelType& value)
{
    if (mPixels)
    {
        for (int i = 0; i < mNumPixels; ++i)
        {
            mPixels[0][0][i] = value;
        }
    }
}
//----------------------------------------------------------------------------
template <typename PixelType>
void Image3<PixelType>::AllocatePointers ()
{
    if (mPixels)
    {
        delete1(mPixels[0]);
        delete1(mPixels);
    }

    mPixels = new1<PixelType**>(mDimensions[2]);
    mPixels[0] = new1<PixelType*>(mDimensions[1]*mDimensions[2]);
    mPixels[0][0] = (PixelType*)mRawPixels;
    for (int i2 = 0; i2 < mDimensions[2]; ++i2)
    {
        int j1 = mDimensions[1]*i2;  // = mDimensions[1]*(i2 + j2), j2 = 0
        mPixels[i2] = &mPixels[0][j1];
        for (int i1 = 0; i1 < mDimensions[1]; ++i1)
        {
            int j0 = mDimensions[0]*(i1 + j1);
            mPixels[i2][i1] = &mPixels[0][0][j0];
        }
    }
}
//----------------------------------------------------------------------------
template <typename PixelType>
void Image3<PixelType>::DeallocatePointers ()
{
    if (mPixels)
    {
        delete1(mPixels[0]);
        delete1(mPixels);
        mPixels = 0;
    }
}
//----------------------------------------------------------------------------
