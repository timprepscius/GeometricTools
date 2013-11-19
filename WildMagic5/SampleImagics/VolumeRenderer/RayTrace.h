// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef RAYTRACE_H
#define RAYTRACE_H

#include "Wm5Images.h"
using namespace Wm5;

class RayTrace
{
public:
    RayTrace (ImageUChar3D* image, float gamma);
    ~RayTrace ();

    bool MoveTrackBall (float x0, float y0, float x1, float y1);
    void Trace (int spacing);
    void DrawWireFrame ();
    void Correction (float gamma);
    inline Ergb8 GetRendered (int x, int y);
    inline Ergb8 GetRendered (int i);
    inline float& Frame (int row, int col);

private:
    // Storage for scaled volume data.
    ImageFloat3D* mDensity;

    // Accumulator and render images.
    int mBound, mBoundM1, mHBound;
    ImageFloat2D* mAccum;
    ImageRGB82D* mRender;

    // Center point of image.
    float mXCenter, mYCenter, mZCenter;

    // For gamma correction of rendered image values.
    float mGamma;

    // Frame field for eyepoint:  u = column 0, v = column 1, w = column 2.
    float mFrame[3][3];

    bool Clipped (float p, float q, float& u1, float& u2);
    bool Clip3D (float& x1, float& y1, float& z1, float& x2, float& y2,
        float& z2);
    void Line3D (int j0, int j1, int x0, int y0, int z0, int x1, int y1,
        int z1);
    void Line2D (bool visible, int x0, int y0, int x1, int y1);
};

#include "RayTrace.inl"

#endif
