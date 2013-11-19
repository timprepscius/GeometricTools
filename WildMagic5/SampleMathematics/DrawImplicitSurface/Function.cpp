// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Function.h"

// Define only one of these.
//#define F_SPHERE
#define F_TORUS

#ifdef F_SPHERE
//----------------------------------------------------------------------------
float F (const Vector3f& position)
{
    // F(x,y,z) = x^2 + y^2 + z^2 - 1 (level surface is a sphere)
    return position.SquaredLength() - 1.0f;
}
//----------------------------------------------------------------------------
Vector3f DF (const Vector3f& position)
{
    // DF(x,y,z) = (2x,2y,2z)
    return 2.0f*position;
}
//----------------------------------------------------------------------------
#endif

#ifdef F_TORUS
// Ro > 0 is radius from center of torus
// Ri > 0 is radius of tube of torus
// p^2 = x^2+y^2+z^2
// p^4-2*(Ro^2+Ri^2)*p^2+4*Ro^2*z^2+(Ro^2-Ri^2)^2 = 0
const float gRO = 1.0f;
const float gRI = 0.25f;
const float gROSqr = gRO*gRO;
const float gRISqr = gRI*gRI;
const float gSum = gROSqr + gRISqr;
const float gDiff = gROSqr - gRISqr;
const float gDiffSqr = gDiff*gDiff;
//----------------------------------------------------------------------------
float F (const Vector3f& position)
{
    float sqrLength = position.SquaredLength();
    return sqrLength*(sqrLength - 2.0f*gSum) +
        4.0f*gROSqr*position.Z()*position.Z() + gDiffSqr;
}
//----------------------------------------------------------------------------
Vector3f DF (const Vector3f& position)
{
    Vector3f gradient;

    float temp = position.SquaredLength() - gSum;
    gradient.X() = 4.0f*position.X()*temp; 
    gradient.Y() = 4.0f*position.Y()*temp; 
    gradient.Z() = 4.0f*position.Z()*(temp + 2.0f*gROSqr);

    return gradient;
}
//----------------------------------------------------------------------------
#endif
