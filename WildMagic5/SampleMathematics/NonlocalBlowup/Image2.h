// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

#ifndef IMAGE2_H
#define IMAGE2_H

#include "Image.h"
#include "Wm5Memory.h"
#include "Wm5Tuple.h"
using namespace Wm5;

// The PixelType must be "plain old data" (POD) and cannot have side effects
// from construction or destruction.

template <typename PixelType>
class Image2 : public Image
{
public:
    // The default constructor creates a null image.  You can resize the image
    // later with an explicit Resize call or an assignment.
    Image2 ();
    virtual ~Image2 ();

    // Copy the input image using the assignment operator.
    Image2 (const Image2& image);

    // The input dimensions must be positive; otherwise, a null image is
    // created.
    Image2 (int dimension0, int dimension1);

    // If the input image is compatible with 'this', a copy of the input
    // image data occurs.  If the input image is not compatible, 'this' is
    // recreated to be a copy of 'image'.
    Image2& operator= (const Image2& image);

    // Conversion between 1-dimensional indices and 2-dimensional coordinates.
    inline int GetIndex (int x, int y) const;
    inline int GetIndex (const Tuple<2,int>& coord) const;
    inline void GetCoordinates (int index, int& x, int& y) const;
    inline Tuple<2,int> GetCoordinates (int index) const;

    // Access the data as a 1-dimensional array.  The operator[] functions
    // test for valid i in debug configurations and assert on invalid i.  The
    // Get() functions test for valid i and clamp when invalid (debug and
    // release); these functions cannot fail.
    inline PixelType* GetPixels1D () const;
    inline PixelType& operator[] (int i);
    inline const PixelType& operator[] (int i) const;
    PixelType& Get (int i);
    const PixelType& Get (int i) const;

    // Access the data as a 2-dimensional array.  Pixel (x,y) is accessed
    // as "pixels2D[y][x]".  The operator() functions test for valid (x,y) in
    // debug configurations and assert on invalid (x,y).  The Get() functions
    // test for valid (x,y) and clamp when invalid (debug and release); these
    // functions cannot fail.
    inline PixelType** GetPixels2D () const;
    inline PixelType& operator() (int x, int y);
    inline const PixelType& operator() (int x, int y) const;
    inline PixelType& operator() (const Tuple<2,int>& coord);
    inline const PixelType& operator() (const Tuple<2,int>& coord) const;
    inline PixelType& Get (int x, int y);
    inline const PixelType& Get (int x, int y) const;
    inline PixelType& Get (Tuple<2,int> coord);
    inline const PixelType& Get (Tuple<2,int> coord) const;

    // Resize an image.  All data is lost from the original image.  The
    // function is convenient for taking a default-constructed image and
    // setting its dimension once it is known.  This avoids an irrelevant
    // memory copy that occurs if instead you were to use the statement
    // image = Image1<PixelType>(dimension0, dimension1).  The return value
    // is 'true' whenever the image is resized (reallocations occurred).
    bool Resize (int dimension0, int dimension1);

    // Set all pixels to the specified value.
    void SetAllPixels (const PixelType& value);

private:
    void AllocatePointers ();
    void DeallocatePointers ();

    // Typed pointers to Image::mRawPixels.
    PixelType** mPixels;

    // Uninitialized, used in the Get(int) calls.
    PixelType mInvalidPixel;
};

#include "Image2.inl"

#endif
