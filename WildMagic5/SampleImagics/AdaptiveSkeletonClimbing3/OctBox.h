// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.5.0 (2010/10/23)

#ifndef OCTBOX_H
#define OCTBOX_H

class OctBox
{
public:
    // Construction.
    OctBox (int x0, int y0, int z0, int dx, int dy, int dz, int LX, int LY,
        int LZ)
        :
        mX0(x0),
        mY0(y0),
        mZ0(z0),
        mX1(x0 + dx),
        mY1(y0 + dy),
        mZ1(z0 + dz),
        mDX(dx),
        mDY(dy),
        mDZ(dz),
        mLX(LX),
        mLY(LY),
        mLZ(LZ)
    {
    }

    int mX0, mY0, mZ0, mX1, mY1, mZ1, mDX, mDY, mDZ, mLX, mLY, mLZ;
};

#endif
