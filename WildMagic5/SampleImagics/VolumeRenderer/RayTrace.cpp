// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "RayTrace.h"
#include "Wm5Math.h"

//----------------------------------------------------------------------------
RayTrace::RayTrace (ImageUChar3D* image, float gamma)
    :
    mGamma(gamma)
{
    // Get the maximum bound.
    int maxBound = image->GetBound(0);
    if (image->GetBound(1) > maxBound)
    {
        maxBound = image->GetBound(1);
    }
    if (image->GetBound(2) > maxBound)
    {
        maxBound = image->GetBound(2);
    }

    // Construct density in range [0,1/maxBound].  The accumulator values
    // must not exceed 1.
    mDensity = new0 ImageFloat3D(image->GetBound(0), image->GetBound(1),
        image->GetBound(2));
    unsigned char minValue = (*image)[0], maxValue = minValue;
    int i;
    for (i = 1; i < image->GetQuantity(); ++i)
    {
        if ((*image)[i] < minValue)
        {
            minValue = (*image)[i];
        }
        else if ((*image)[i] > maxValue)
        {
            maxValue = (*image)[i];
        }
    }
    float fMinValue = (float)minValue;
    float fMaxValue = (float)maxValue;
    float invRange = 1.0f/((fMaxValue - fMinValue)*maxBound);
    for (i = 1; i < image->GetQuantity(); ++i)
    {
        (*mDensity)[i] = ((*image)[i]- fMinValue)*invRange;
    }

    // Center point of 3D image.
    mXCenter = (float)((mDensity->GetBound(0) - 1)/2);
    mYCenter = (float)((mDensity->GetBound(1) - 1)/2);
    mZCenter = (float)((mDensity->GetBound(2) - 1)/2);

    // Determine image bounds and allocate images.
    mBound = (int)Mathf::Ceil(2.0f*maxBound);
    if (mBound % 2)
    {
        mBound++;
    }
    mBoundM1 = mBound - 1;
    mHBound = mBound/2;
    mAccum = new0 ImageFloat2D(mBound, mBound);
    mRender = new0 ImageRGB82D(mBound, mBound);

    // Initialize eyepoint frame field.
    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 3; col++)
        {
            mFrame[row][col] = (row == col ? 1.0f : 0.0f);
        }
    }
}
//----------------------------------------------------------------------------
RayTrace::~RayTrace ()
{
    delete0(mDensity);
    delete0(mAccum);
    delete0(mRender);
}
//----------------------------------------------------------------------------
bool RayTrace::Clipped (float p, float q, float& u0, float& u1)
{
    float fR;

    if (p < 0.0f)
    {
        fR = q/p;
        if (fR > u1)
        {
            return false;
        }
        if (fR > u0)
        {
            u0 = fR;
        }
        return true;
    }

    if (p > 0.0f)
    {
        fR = q/p;
        if (fR < u0)
        {
            return false;
        }
        if (fR < u1)
        {
            u1 = fR;
        }
        return true;
    }

    // p == 0
    return q >= 0.0f;
}
//----------------------------------------------------------------------------
bool RayTrace::Clip3D (float& x0, float& y0, float& z0, float& x1,
    float& y1, float& z1)
{
    // The clipping region.
    const float xmin = -mXCenter, xmax = mXCenter;
    const float ymin = -mYCenter, ymax = mYCenter;
    const float zmin = -mZCenter, zmax = mZCenter;

    float u0 = 0.0f, u1 = 1.0f;

    float dx = x1 - x0;
    if (!Clipped(-dx, x0 - xmin, u0, u1) || !Clipped(+dx, xmax - x0, u0, u1))
    {
        return false;
    }

    float dy = y1 - y0;
    if (!Clipped(-dy, y0 - ymin, u0, u1) || !Clipped(+dy, ymax - y0, u0, u1))
    {
        return false;
    }

    float dz = z1 - z0;
    if (!Clipped(-dz, z0 - zmin, u0, u1) || !Clipped(+dz, zmax - z0, u0, u1))
    {
        return false;
    }

    if (u1 < 1.0f)
    {
        x1 = x0 + u1*dx;
        y1 = y0 + u1*dy;
        z1 = z0 + u1*dz;
    }
    if (u0 > 0.0f)
    {
        x0 = x0 + u0*dx;
        y0 = y0 + u0*dy;
        z0 = z0 + u0*dz;
    }

    return true;
}
//----------------------------------------------------------------------------
void RayTrace::Line3D (int j0, int j1, int x0, int y0, int z0, int x1,
    int y1, int z1)
{
    // Starting point of line.
    int x = x0, y = y0, z = z0;

    // Direction of line.
    int dx = x1 - x0, dy = y1 - y0, dz = z1 - z0;

    // Increment or decrement depending on direction of line.
    int sx = (dx > 0 ? 1 : (dx < 0 ? -1 : 0));
    int sy = (dy > 0 ? 1 : (dy < 0 ? -1 : 0));
    int sz = (dz > 0 ? 1 : (dz < 0 ? -1 : 0));

    // Decision parameters for voxel selection.
    dx = abs(dx);
    dy = abs(dy);
    dz = abs(dz);
    int ax = 2*dx, ay = 2*dy, az = 2*dz;
    int xDec, yDec, zDec;

    // Determine largest direction component and single-step related variable.
    int dmax = dx, var = 0;
    if (dy > dmax)
    {
        dmax = dy;
        var = 1;
    }
    if (dz > dmax)
    {
        dmax = dz;
        var = 2;
    }

    // Traverse Bresenham line and accumulate density values.
    int index = j0 + mAccum->GetBound(0)*j1;
    switch (var)
    {
    case 0:  // Single-step in x-direction.
        yDec = ay - dx;
        zDec = az - dx;
        for (/**/; /**/; x += sx, yDec += ay, zDec += az)
        {
            // Accumulate the density value.
            (*mAccum)[index] = (*mAccum)[index] + (*mDensity)(x, y, z);

            // Take Bresenham step.
            if (x == x1)
            {
                break;
            }
            if (yDec >= 0)
            {
                yDec -= ax;
                y += sy;
            }
            if (zDec >= 0)
            {
                zDec -= ax;
                z += sz;
            }
        }
        break;
    case 1:  // Single-step in y-direction.
        xDec = ax - dy;
        zDec = az - dy;
        for (/**/; /**/; y += sy, xDec += ax, zDec += az)
        {
            // Accumulate the density value.
            (*mAccum)[index] = (*mAccum)[index] + (*mDensity)(x, y, z);

            // Take Bresenham step.
            if (y == y1)
            {
                break;
            }
            if (xDec >= 0)
            {
                xDec -= ay;
                x += sx;
            }
            if (zDec >= 0)
            {
                zDec -= ay;
                z += sz;
            }
        }
        break;
    case 2:  // Single-step in z-direction.
        xDec = ax - dz;
        yDec = ay - dz;
        for (/**/; /**/; z += sz, xDec += ax, yDec += ay)
        {
            // Accumulate the density value.
            (*mAccum)[index] = (*mAccum)[index] + (*mDensity)(x, y, z);

            // Take Bresenham step.
            if (z == z1)
            {
                break;
            }
            if (xDec >= 0)
            {
                xDec -= az;
                x += sx;
            }
            if (yDec >= 0)
            {
                yDec -= az;
                y += sy;
            }
        }
        break;
    }

    unsigned char gray =
        (unsigned char)(255.0f*Mathf::Pow((*mAccum)[index], mGamma));
    (*mRender)[index] = GetColor24(gray, gray, gray);
}
//----------------------------------------------------------------------------
void RayTrace::Trace (int spacing)
{
    *mAccum = 0.0f;
    *mRender = (Ergb8)0;

    float w0 = mHBound*mFrame[0][2];
    float w1 = mHBound*mFrame[1][2];
    float w2 = mHBound*mFrame[2][2];
    for (int i1 = -mHBound; i1 < mHBound; i1 += spacing)
    {
        int j1 = i1 + mHBound;
        for (int i0 = -mHBound; i0 < mHBound; i0 += spacing)
        {
            int j0 = i0 + mHBound;

            float a0 = i0*mFrame[0][0] + i1*mFrame[0][1];
            float a1 = i0*mFrame[1][0] + i1*mFrame[1][1];
            float a2 = i0*mFrame[2][0] + i1*mFrame[2][1];

            float fx0 = a0 - w0, fy0 = a1 - w1, fz0 = a2 - w2;
            float fx1 = a0 + w0, fy1 = a1 + w1, fz1 = a2 + w2;

            if (Clip3D(fx0, fy0, fz0, fx1, fy1, fz1))
            {
                int x0 = (int)(fx0 + mXCenter);
                int y0 = (int)(fy0 + mYCenter);
                int z0 = (int)(fz0 + mZCenter);
                int x1 = (int)(fx1 + mXCenter);
                int y1 = (int)(fy1 + mYCenter);
                int z1 = (int)(fz1 + mZCenter);
                Line3D(j0, j1, x0, y0, z0, x1, y1, z1);
                if (spacing > 1)
                {
                    unsigned int value = (*mRender)(j0, j1);
                    for (int y = 0; y < spacing; ++y)
                    {
                        for (int x = 0; x < spacing; ++x)
                        {
                            (*mRender)(j0 + x, j1 + y) = value;
                        }
                    }
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
void RayTrace::DrawWireFrame ()
{
    // Calculate orientation of cube corners relative to u and v.
    float udot0 =  mFrame[0][0] + mFrame[1][0] + mFrame[2][0];
    float udot1 =  mFrame[0][0] + mFrame[1][0] - mFrame[2][0];
    float udot2 = -mFrame[0][0] + mFrame[1][0] - mFrame[2][0];
    float udot3 = -mFrame[0][0] + mFrame[1][0] + mFrame[2][0];
    float vdot0 =  mFrame[0][1] + mFrame[1][1] + mFrame[2][1];
    float vdot1 =  mFrame[0][1] + mFrame[1][1] - mFrame[2][1];
    float vdot2 = -mFrame[0][1] + mFrame[1][1] - mFrame[2][1];
    float vdot3 = -mFrame[0][1] + mFrame[1][1] + mFrame[2][1];

    // Corner locations.
    int x[8],  y[8];

    // (-1, -1, -1)
    x[0] = (int)(mBoundM1*(1.0f - 0.5f*udot0)*0.5f);
    y[0] = (int)(mBoundM1*(1.0f - 0.5f*vdot0)*0.5f);

    // (1, -1, -1)
    x[1] = (int)(mBoundM1*(1.0f - 0.5f*udot3)*0.5f);
    y[1] = (int)(mBoundM1*(1.0f - 0.5f*vdot3)*0.5f);

    // (1, 1, -1)
    x[2] = (int)(mBoundM1*(1.0f + 0.5f*udot1)*0.5f);
    y[2] = (int)(mBoundM1*(1.0f + 0.5f*vdot1)*0.5f);

    // (-1, 1, -1)
    x[3] = (int)(mBoundM1*(1.0f + 0.5f*udot2)*0.5f);
    y[3] = (int)(mBoundM1*(1.0f + 0.5f*vdot2)*0.5f);

    // (-1, -1, 1)
    x[4] = (int)(mBoundM1*(1.0f - 0.5f*udot1)*0.5f);
    y[4] = (int)(mBoundM1*(1.0f - 0.5f*vdot1)*0.5f);

    // (1, -1, 1)
    x[5] = (int)(mBoundM1*(1.0f - 0.5f*udot2)*0.5f);
    y[5] = (int)(mBoundM1*(1.0f - 0.5f*vdot2)*0.5f);

    // (1, 1, 1)
    x[6] = (int)(mBoundM1*(1.0f + 0.5f*udot0)*0.5f);
    y[6] = (int)(mBoundM1*(1.0f + 0.5f*vdot0)*0.5f);

    // (-1, 1, 1)
    x[7] = (int)(mBoundM1*(1.0f + 0.5f*udot3)*0.5f);
    y[7] = (int)(mBoundM1*(1.0f + 0.5f*vdot3)*0.5f);

    // Draw back faces.
    if (mFrame[0][2] < 0.0f)
    {
        // face x = -1
        Line2D(false, x[0], y[0], x[3], y[3]);
        Line2D(false, x[3], y[3], x[7], y[7]);
        Line2D(false, x[7], y[7], x[4], y[4]);
        Line2D(false, x[4], y[4], x[0], y[0]);
    }
    else if (mFrame[0][2] > 0.0f)
    {
        // face x = +1
        Line2D(false, x[1], y[1], x[2], y[2]);
        Line2D(false, x[2], y[2], x[6], y[6]);
        Line2D(false, x[6], y[6], x[5], y[5]);
        Line2D(false, x[5], y[5], x[1], y[1]);
    }

    if (mFrame[1][2] < 0.0f)
    {
        // face y = -1
        Line2D(false, x[0], y[0], x[1], y[1]);
        Line2D(false, x[1], y[1], x[5], y[5]);
        Line2D(false, x[5], y[5], x[4], y[4]);
        Line2D(false, x[4], y[4], x[0], y[0]);
    }
    else if (mFrame[1][2] > 0.0f)
    {
        // face y = +1
        Line2D(false, x[2], y[2], x[3], y[3]);
        Line2D(false, x[3], y[3], x[7], y[7]);
        Line2D(false, x[7], y[7], x[6], y[6]);
        Line2D(false, x[6], y[6], x[2], y[2]);
    }

    if (mFrame[2][2] < 0.0f)
    {
        // face z = -1
        Line2D(false, x[0], y[0], x[1], y[1]);
        Line2D(false, x[1], y[1], x[2], y[2]);
        Line2D(false, x[2], y[2], x[3], y[3]);
        Line2D(false, x[3], y[3], x[0], y[0]);
    }
    else if (mFrame[2][2] > 0.0f)
    {
        // face z = +1
        Line2D(false, x[4], y[4], x[5], y[5]);
        Line2D(false, x[5], y[5], x[6], y[6]);
        Line2D(false, x[6], y[6], x[7], y[7]);
        Line2D(false, x[7], y[7], x[4], y[4]);
    }

    // Fraw front faces.
    if (mFrame[0][2] < 0.0f)
    {
        // face x = +1
        Line2D(true, x[1], y[1], x[2], y[2]);
        Line2D(true, x[2], y[2], x[6], y[6]);
        Line2D(true, x[6], y[6], x[5], y[5]);
        Line2D(true, x[5], y[5], x[1], y[1]);
    }
    else if (mFrame[0][2] > 0.0f)
    {
        // face x = -1
        Line2D(true, x[0], y[0], x[3], y[3]);
        Line2D(true, x[3], y[3], x[7], y[7]);
        Line2D(true, x[7], y[7], x[4], y[4]);
        Line2D(true, x[4], y[4], x[0], y[0]);
    }

    if (mFrame[1][2] < 0.0f)
    {
        // face y = +1
        Line2D(true, x[2], y[2], x[3], y[3]);
        Line2D(true, x[3], y[3], x[7], y[7]);
        Line2D(true, x[7], y[7], x[6], y[6]);
        Line2D(true, x[6], y[6], x[2], y[2]);
    }
    else if (mFrame[1][2] > 0.0f)
    {
        // face y = -1
        Line2D(true, x[0], y[0], x[1], y[1]);
        Line2D(true, x[1], y[1], x[5], y[5]);
        Line2D(true, x[5], y[5], x[4], y[4]);
        Line2D(true, x[4], y[4], x[0], y[0]);
    }

    if (mFrame[2][2] < 0.0f)
    {
        // face z = +1
        Line2D(true, x[4], y[4], x[5], y[5]);
        Line2D(true, x[5], y[5], x[6], y[6]);
        Line2D(true, x[6], y[6], x[7], y[7]);
        Line2D(true, x[7], y[7], x[4], y[4]);
    }
    else if (mFrame[2][2] > 0.0f)
    {
        // face z = -1
        Line2D(true, x[0], y[0], x[1], y[1]);
        Line2D(true, x[1], y[1], x[2], y[2]);
        Line2D(true, x[2], y[2], x[3], y[3]);
        Line2D(true, x[3], y[3], x[0], y[0]);
    }
}
//----------------------------------------------------------------------------
void RayTrace::Line2D (bool visible, int x0, int y0, int x1, int y1)
{
    // Starting point of line.
    int x = x0, y = y0;

    // Direction of line.
    int dx = x1-x0, dy = y1-y0;

    // Increment or decrement depending on direction of line.
    int sx = (dx > 0 ? 1 : (dx < 0 ? -1 : 0));
    int sy = (dy > 0 ? 1 : (dy < 0 ? -1 : 0));

    // Decision parameters for pixel selection.
    dx = abs(dx);
    dy = abs(dy);
    int ax = 2*dx, ay = 2*dy;
    int xDec, yDec;

    // Determine largest direction component and single-step related variable.
    int dmax = dx, var = 0;
    if (dy > dmax)
    {
        dmax = dy;
        var = 1;
    }

    // Traverse Bresenham line and set render colors.
    switch (var)
    {
    case 0:  // Single-step in x-direction.
        yDec = ay - dx;
        for (/**/; /**/; x += sx, yDec += ay)
        {
            // Process pixel (x,y).
            if (visible)
            {
                (*mRender)(x, y) = GetColor24(255, 0, 0);
            }
            else
            {
                (*mRender)(x, y) = GetColor24(128, 0, 0);
            }


            // Take Bresenham step.
            if (x == x1)
            {
                break;
            }
            if (yDec >= 0)
            {
                yDec -= ax;
                y += sy;
            }
        }
        break;
    case 1:  // Single-step in y-direction.
        xDec = ax - dy;
        for (/**/; /**/; y += sy, xDec += ax)
        {
            // Process pixel (x,y).
            if (visible)
            {
                (*mRender)(x, y) = GetColor24(255, 0, 0);
            }
            else
            {
                (*mRender)(x, y) = GetColor24(128, 0, 0);
            }

            // Take Bresenham step.
            if (y == y1)
            {
                break;
            }
            if (xDec >= 0)
            {
                xDec -= ay;
                x += sx;
            }
        }
        break;
    }
}
//----------------------------------------------------------------------------
bool RayTrace::MoveTrackBall (float x0, float y0, float x1, float y1)
{
    if (x0 == x1 && y0 == y1)
    {
        // No rotation, because the trackball has not moved.
        return false;
    }

    float length = Mathf::Sqrt(x0*x0 + y0*y0), z0, z1, invLength;
    if (length > 1.0f)
    {
        // Outside unit disk, project onto it.
        invLength = 1.0f/length;
        x0 *= invLength;
        y0 *= invLength;
        z0 = 0.0f;
    }
    else
    {
        // Compute point (x0,y0,z0) on negative unit hemisphere.
        z0 = 1.0f - x0*x0 - y0*y0;
        z0 = (z0 <= 0.0f ? 0.0f : Mathf::Sqrt(z0));
    }
    z0 *= -1.0f;

    length = Mathf::Sqrt(x1*x1 + y1*y1);
    if (length > 1.0f)
    {
        // Outside unit disk, project onto it.
        invLength = 1.0f/length;
        x1 *= invLength;
        y1 *= invLength;
        z1 = 0.0f;
    }
    else
    {
        // Compute point (x1,y1,z1) on negative unit hemisphere.
        z1 = 1.0f - x1*x1 - y1*y1;
        z1 = (z1 <= 0.0f ? 0.0f : Mathf::Sqrt(z1));
    }
    z1 *= -1.0f;

    // Create unit direction vector to rotate about.
    float dir[3] = { y0*z1 - y1*z0, x1*z0 - x0*z1, x0*y1 - x1*y0 };
    length = Mathf::Sqrt(dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2]);
    if (length == 0.0f)
    {
        // Rotated pi radians.
        length = Mathf::Sqrt(x0*x0 + y0*y0);
        invLength = 1.0f/length;
        dir[0] = y0*invLength;
        dir[1] = -x0*invLength;
        dir[2] = 0.0f;
    }
    else
    {
        invLength = 1.0f/length;
        dir[0] *= invLength;
        dir[1] *= invLength;
        dir[2] *= invLength;
    }

    // Tensor product of direction vector with itself.
    float d00 = dir[0]*dir[0];
    float d01 = dir[0]*dir[1];
    float d02 = dir[0]*dir[2];
    float d11 = dir[1]*dir[1];
    float d12 = dir[1]*dir[2];
    float d22 = dir[2]*dir[2];

    // Cosine and sine of angle of rotation about direction vector.
    float cs = x0*x1 + y0*y1 + z0*z1;
    if (cs < -1.0f)
    {
        cs = -1.0f;
    }
    else if (cs > 1.0f)
    {
        cs = 1.0f;
    }
    float sn = Mathf::Sqrt(Mathf::FAbs(1.0f - cs*cs));
    float oneMinusCs = 1.0f - cs;

    // rotation matrix implied by track ball motion
    float rot[3][3] =
    {
        {
            oneMinusCs*d00 + cs,
            oneMinusCs*d01 - sn*dir[2],
            oneMinusCs*d02 + sn*dir[1]
        },
        {
            oneMinusCs*d01 + sn*dir[2],
            oneMinusCs*d11 + cs,
            oneMinusCs*d12 - sn*dir[0]
        },
        {
            oneMinusCs*d02 - sn*dir[1],
            oneMinusCs*d12 + sn*dir[0],
            oneMinusCs*d22 + cs
        }
    };

    // Rotate the eyepoint frame field.
    int row, col;
    float tmp[3][3] =
    {
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}
    };

    for (row = 0; row < 3; ++row)
    {
        for (col = 0; col < 3; ++col)
        {
            for (int mid = 0; mid < 3; ++mid)
            {
                tmp[row][col] += mFrame[row][mid]*rot[col][mid];
            }
        }
    }

    // Renormalize using Gram-Schmidt algorithm to avoid degeneration.
    // If the matrix columns are v0, v1, and v2, then the Gram-Schmidt
    // orthonormalization produces new columns u0, u1, and u2 as follows,
    //
    //   u0 = v0/|v0|
    //   u1 = (v1-(u0*v1)u0)/|v1-(u0*v1)u0|
    //   u2 = (v2-(u0*v2)u0-(u1*v2)u1)/|v2-(u0*v2)u0-(u1*v2)u1|
    //
    // where |A| indicates length of vector A and A*B indicates dot
    // product of vectors A and B.
    float dot0, dot1;

    // Compute u0.
    length = tmp[0][0]*tmp[0][0] + tmp[1][0]*tmp[1][0] + tmp[2][0]*tmp[2][0];
    invLength = 1.0f/length;
    tmp[0][0] *= invLength;
    tmp[1][0] *= invLength;
    tmp[2][0] *= invLength;

    // Compute u1.
    dot0 = tmp[0][0]*tmp[0][1] + tmp[1][0]*tmp[1][1] + tmp[2][0]*tmp[2][1];
    tmp[0][1] -= dot0*tmp[0][0];
    tmp[1][1] -= dot0*tmp[1][0];
    tmp[2][1] -= dot0*tmp[2][0];
    length = tmp[0][1]*tmp[0][1] + tmp[1][1]*tmp[1][1] + tmp[2][1]*tmp[2][1];
    invLength = 1.0f/length;
    tmp[0][1] *= invLength;
    tmp[1][1] *= invLength;
    tmp[2][1] *= invLength;

    // Compute u2.
    dot0 = tmp[0][0]*tmp[0][2] + tmp[1][0]*tmp[1][2] + tmp[2][0]*tmp[2][2];
    dot1 = tmp[0][1]*tmp[0][2] + tmp[1][1]*tmp[1][2] + tmp[2][1]*tmp[2][2];
    tmp[0][2] -= dot0*tmp[0][0] + dot1*tmp[0][1];
    tmp[1][2] -= dot0*tmp[1][0] + dot1*tmp[1][1];
    tmp[2][2] -= dot0*tmp[2][0] + dot1*tmp[2][1];
    length = tmp[0][2]*tmp[0][2] + tmp[1][2]*tmp[1][2] + tmp[2][2]*tmp[2][2];
    invLength = 1.0f/length;
    tmp[0][2] *= invLength;
    tmp[1][2] *= invLength;
    tmp[2][2] *= invLength;

    // Copy the temporary frame to the permanent one.
    for (row = 0; row < 3; ++row)
    {
        for (col = 0; col < 3; ++col)
        {
            mFrame[row][col] = tmp[row][col];
        }
    }

    return true;
}
//----------------------------------------------------------------------------
void RayTrace::Correction (float gamma)
{
    mGamma = gamma;
    for (int i = 0; i < mAccum->GetQuantity(); ++i)
    {
        unsigned char gray =
            (unsigned char)(31.0f*Mathf::Pow((*mAccum)[i], mGamma));

        (*mRender)[i] = GetColor24(gray, gray, gray);
    }
}
//----------------------------------------------------------------------------
