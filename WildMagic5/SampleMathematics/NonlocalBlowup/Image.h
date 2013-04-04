// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

#ifndef IMAGE_H
#define IMAGE_H

//#define IMAGE_RANGE_CHECK
#include <string>

// The pixel data type must be "plain old data" (POD) and cannot have side
// effects from construction or destruction.  Classes derived from Image
// cannot have construction or destruction side effects.

class Image
{
public:
    // Abstract base class.
    virtual ~Image ();
protected:
    Image ();
    Image (const Image& image);
    Image (int pixelSize, int numDimensions, ...);

    // This function asserts when called.  Use Copy(...) instead.
    Image& operator= (const Image& image);

    bool Copy (const Image& image);

public:
    // Access to image data.
    inline int GetPixelSize () const;
    inline int GetNumDimensions () const;
    inline const int* GetDimensions () const;
    int GetDimension (int i) const;
    inline const int* GetOffsets () const;
    int GetOffset (int i) const;
    inline int GetNumPixels () const;
    inline const char* GetRawPixels () const;
    inline bool IsNullImage () const;

    // Conversions between n-dim and 1-dim structures.  The 'coord' arrays
    // must have GetNumDimensions() elements.
    int GetIndex (const int* coord) const;
    void GetCoordinates (int index, int* coord) const;

    // Set all image data to zero (using a memset).  The metadata is
    // unaffected by this call.
    void ClearPixels ();

protected:
    // Deallocate arrays and initialize all data members to zero.
    void CreateNullImage ();

    // Reallocate the image to match the requested dimensions.  If the image
    // is compatible with the inputs, nothing is modified and the image data
    // remains intact; otherwise, reallocations occur and all image data and
    // metadata information is lost by the operation.  The return value is
    // 'true' whenever the image is resized (the image is not compatible with
    // the inputs).
    bool Resize (int numDimensions, ...);

    // Two images are compatible when they have the same number of dimensions,
    // the same dimensions, and the same pixel type.
    bool IsCompatible (const Image& image) const;

    int mPixelSize;
    int mNumDimensions;
    int* mDimensions;
    int* mOffsets;
    int mNumPixels;

    // Pixel data.
    char* mRawPixels;
};

#include "Image.inl"

#endif
