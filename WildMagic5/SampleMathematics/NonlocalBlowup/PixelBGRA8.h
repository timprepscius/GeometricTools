// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.3.0 (2010/09/07)

#ifndef PIXELBGRA8_H
#define PIXELBGRA8_H

class PixelBGRA8
{
public:
    inline PixelBGRA8 ()
    {
        // Uninitialized;
    }

    inline PixelBGRA8 (unsigned char inB, unsigned char inG,
        unsigned char inR, unsigned char inA)
        :
        b(inB), g(inG), r(inR), a(inA)
    {
    }

    inline bool operator== (const PixelBGRA8& color) const
    {
        return b == color.b && g == color.g && r == color.r && a == color.a;
    }

    inline bool operator!= (const PixelBGRA8& color) const
    {
        return b != color.b || g != color.g || r != color.r || a != color.a;
    }

    unsigned char b, g, r, a;
};

#endif
