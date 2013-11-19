// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "RayTracer.h"
#include "Wm5Memory.h"

//----------------------------------------------------------------------------
RayTracer::RayTracer (Function F, Gradient DF, int width, int height)
    :
    Location(0.0f, 0.0f, 0.0f),
    DVector(0.0f, 0.0f, 1.0f),
    UVector(0.0f, 1.0f, 0.0f),
    RVector(-1.0f, 0.0f, 0.0f),
    Near(1.0f),
    Far(10.0f),
    HalfWidth(0.5f),
    HalfHeight(0.5f),
    Epsilon(1.0e-4f),
    MaxBisectSteps(8),
    mFunction(F),
    mGradient(DF),
    mWidth(width),
    mHeight(height)
{
    int numPixels = mWidth*mHeight;
    mImage = new1<float>(numPixels);
    mBlurredImage = new1<float>(numPixels);
    size_t numBytes = numPixels*sizeof(float);
    memset(mImage, 0, numBytes);
    memset(mBlurredImage, 0, numBytes);
}
//----------------------------------------------------------------------------
RayTracer::~RayTracer ()
{
    delete1(mImage);
    delete1(mBlurredImage);
}
//----------------------------------------------------------------------------
void RayTracer::DrawSurface (int numSamples, const Vector3f& lightDirection,
    bool blur)
{
    float xMult = 2.0f/(mWidth - 1.0f);
    float yMult = 2.0f/(mHeight - 1.0f);
    float ds = (Far - Near)/(Near*(numSamples - 1.0f));

    // Generate a ray per pixel of the image.
    for (int j = 0; j < mHeight; ++j)
    {
        float y = HalfHeight*(-1.0f + yMult*j);
        for (int i = 0; i < mWidth; ++i)
        {
            float x = HalfWidth*(-1.0f + xMult*i);

            // The background is zero.
            int index = i + mWidth*j;
            mImage[index] = 0.0f;

            // Compute the ray direction.
            Vector3f rayDirection = Near*DVector + x*RVector + y*UVector;

            // Compute the ray origin.
            float s0 = 1.0f;
            Vector3f position0 = Location;
            float f0 = mFunction(position0);

            // Sample the ray.
            for (int k = 0; k < numSamples; ++k)
            {
                // trace ray E+s*V where 1 <= s <= far/near
                float s1 = 1.0f + ds*k;
                Vector3f position1 = Location + s1*rayDirection;
                float f1 = mFunction(position1);
                if (f0*f1 <= 0.0f)
                {
                    // Bisect [s0,s1] to find s for which F(L+s*D) = 0.  This
                    // approach is not accurate near the contour of an object
                    // since there will be multiple roots that are close
                    // together and the uniform ray sampling can miss these
                    // roots.  Either Newton's method should be used (or a
                    // hybrid of bisection and Newton's).
                    Vector3f surfacePosition, surfaceNormal;
                    FindSurface(s0, f0, position0, s1, f1, position1,
                        rayDirection, surfacePosition, surfaceNormal);

                    // Compute the light intensity.
                    float dot = lightDirection.Dot(surfaceNormal);
                    mImage[index] = (dot < 0.0f ? -dot : 0.0f);
                    break;
                }

                // Proceed to the next sample interval.
                s0 = s1;
                position0 = position1;
                f0 = f1;
            }
        }
    }

    if (blur)
    {
        BlurImage();
    }
}
//----------------------------------------------------------------------------
void RayTracer::FindSurface (float s0, float f0, const Vector3f& position0,
    float s1, float f1, const Vector3f& position1,
    const Vector3f& rayDirection, Vector3f& surfacePosition,
    Vector3f& surfaceNormal)
{
    // Check for endpoint zeros.
    if (Mathf::FAbs(f0) <= Epsilon)
    {
        surfacePosition = position0;
        surfaceNormal = mGradient(position0);
        surfaceNormal.Normalize();
        return;
    }

    if (Mathf::FAbs(f1) <= Epsilon)
    {
        surfacePosition = position1;
        surfaceNormal = mGradient(position1);
        surfaceNormal.Normalize();
        return;
    }

    // Bisect the interval [s0,s1].
    int i;
    for (i = 0; i < MaxBisectSteps; ++i)
    {
        float s = 0.5f*(s0 + s1);
        surfacePosition = Location + s*rayDirection;
        float f = mFunction(surfacePosition);
        if (Mathf::FAbs(f) <= Epsilon)
        {
            break;
        }
        
        if (f*f0 < 0.0f)
        {
            s1 = s;
            f1 = f;
        }
        else
        {
            s0 = s;
            f0 = f;
        }
    }

    surfaceNormal = mGradient(surfacePosition);
    surfaceNormal.Normalize();
}
//----------------------------------------------------------------------------
void RayTracer::BlurImage ()
{
    size_t numBytes = mWidth*mHeight*sizeof(float);
    memcpy(mBlurredImage, mImage, numBytes);

    for (int y = 1; y < mHeight - 1; ++y)
    {
        for (int x = 1; x < mWidth - 1; ++x)
        {
            float value = 0.0f;
            for (int dy = -1; dy <= 1; ++dy)
            {
                for (int dx = -1; dx <= 1; ++dx)
                {
                    value += mImage[(x + dx) + mWidth*(y + dy)];
                }
            }
            mBlurredImage[x + mWidth*y] = value/9.0f;
        }
    }

    memcpy(mImage, mBlurredImage,numBytes);
}
//----------------------------------------------------------------------------
