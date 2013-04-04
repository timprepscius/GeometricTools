// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "Wm5Vector3.h"
using namespace Wm5;

class RayTracer
{
public:
    typedef float (*Function)(const Vector3f&);
    typedef Vector3f (*Gradient)(const Vector3f&);

    // Construction and destruction.  Ray trace the level surface F(X) = 0.
    // The surface normals are computed from DF(X), the gradient of F.
    RayTracer (Function F, Gradient DF, int width, int height);
    ~RayTracer ();

    // Camera and view frustum parameters.
    Vector3f Location, DVector, UVector, RVector;
    float Near, Far, HalfWidth, HalfHeight;

    // Access to the rendered image.
    inline int GetWidth () const;
    inline int GetHeight () const;
    inline const float* GetImage () const;

    // Ray trace the view frustum region.  The tracing uses a single
    // directional light.  TODO:  Allow more lights and different light types.
    // The number of samples per ray for computing intersection of rays with
    // the surface is specified by numSamples..
    void DrawSurface (int numSamples, const Vector3f& lightDirection,
        bool blur);

    // Bisection parameters.
    float Epsilon;       // default = 1.0e-4f
    int MaxBisectSteps;  // default = 8

private:
    // Find the intersection of a ray with the surface.
    void FindSurface (float s0, float f0, const Vector3f& pos0, float s1,
        float f1, const Vector3f& pos1, const Vector3f& rayDirection,
        Vector3f& surfacePosition, Vector3f& surfaceNormal);

    // Blur the image for a cheap antialiasing.
    void BlurImage ();

    // The function F and its gradient DF.
    Function mFunction;
    Gradient mGradient;

    // The rendered image.
    int mWidth, mHeight;
    float* mImage;
    float* mBlurredImage;
};

#include "RayTracer.inl"

#endif
