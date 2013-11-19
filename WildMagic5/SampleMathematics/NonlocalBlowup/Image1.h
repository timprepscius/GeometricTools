// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

#ifndef IMAGE1_H
#define IMAGE1_H

#include "Image.h"
#include "Wm5Memory.h"
#include "Wm5Tuple.h"
using namespace Wm5;

// The PixelType must be "plain old data" (POD) and cannot have side effects
// from construction or destruction.

template <typename PixelType>
class Image1 : public Image
{
public:
    // The default constructor creates a null image.  You can resize the image
    // later with an explicit Resize call or an assignment.
    Image1 ();
    virtual ~Image1 ();

    // Copy the input image using the assignment operator.
    Image1 (const Image1& image);

    // The input dimension must be positive; otherwise, a null image is
    // created.
    Image1 (int dimension);

    // If the input image is compatible with 'this', a copy of the input
    // image data occurs.  If the input image is not compatible, 'this' is
    // recreated to be a copy of 'image'.
    Image1& operator= (const Image1& image);

    // Access the data.  The operator[] function tests for valid i in debug
    // configurations and assert on invalid i.  The Get() functions test for
    // valid i and clamp when invalid (debug and release); these functions
    // cannot fail.
    inline PixelType* GetPixels1D () const;
    inline PixelType& operator[] (int i);
    inline const PixelType& operator[] (int i) const;
    PixelType& Get (int i);
    const PixelType& Get (int i) const;

    // Access the data consistent with the functions that occur for images of
    // higher dimension.  The operator() function tests for valid (x) in debug
    // configurations and assert on invalid (x).  The Get() functions test for
    // valid (x) and clamp when invalid (debug and release); these functions
    // cannot fail.
    inline PixelType& operator() (int x);
    inline const PixelType& operator() (int x) const;
    inline PixelType& operator() (const Tuple<1,int>& coord);
    inline const PixelType& operator() (const Tuple<1,int>& coord) const;
    inline PixelType& Get (Tuple<1,int> coord);
    inline const PixelType& Get (Tuple<1,int> coord) const;

    // Resize an image.  All data is lost from the original image.  The
    // function is convenient for taking a default-constructed image and
    // setting its dimension once it is known.  This avoids an irrelevant
    // memory copy that occurs if instead you were to use the statement
    // image = Image1<PixelType>(dimension).  The return value is 'true'
    // whenever the image is resized (reallocations occurred).
    bool Resize (int dimension);

    // Set all pixels to the specified value.
    void SetAllPixels (const PixelType& value);

private:
    // A typed pointer to Image::mRawPixels.
    PixelType* mPixels;

    // Uninitialized, used in the Get(int) calls.
    PixelType mInvalidPixel;
};

#include "Image1.inl"

#endif
