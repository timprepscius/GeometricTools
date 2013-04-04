// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2012/07/07)

#include "Smoke3D.h"
#include "Wm5Memory.h"

//----------------------------------------------------------------------------
template <typename Real>
Smoke3D<Real>::Smoke3D (Real x0, Real y0, Real z0, Real x1, Real y1, Real z1,
    Real dt, Real denViscosity, Real velViscosity, int imax, int jmax,
    int kmax, int numGaussSeidelIterations, bool densityDirichlet,
    int numVortices)
    :
    FLUIDBASE(x0, y0, z0, x1, y1, z1, dt, denViscosity, velViscosity,
        imax, jmax, kmax, numGaussSeidelIterations, densityDirichlet),
    mNumVortices(numVortices),
    mNumActive(0),
    mGravity((Real)0)
{
    mVortexCenter = new1<Vector3<Real> >(mNumVortices);
    mVortexNormal = new1<Vector3<Real> >(mNumVortices);
    mVortexVariance = new1<Real>(mNumVortices);
    mVortexAmplitude = new1<Real>(mNumVortices);
    mTimelessVortex = new4<Vector3<Real> >(mNumVortices, mIMaxP1, mJMaxP1,
        mKMaxP1);
    mTimelessWind = new3<Vector3<Real> >(mIMaxP1, mJMaxP1, mKMaxP1);

    int v;
    for (v = 0; v < mNumVortices; ++v)
    {
        mVortexCenter[v][0] = Math<Real>::IntervalRandom(mX0, mX1);
        mVortexCenter[v][1] = Math<Real>::IntervalRandom(mY0, mY1);
        mVortexCenter[v][2] = Math<Real>::IntervalRandom(mZ0, mZ1);
        mVortexNormal[v][0] = Math<Real>::SymmetricRandom();
        mVortexNormal[v][1] = Math<Real>::SymmetricRandom();
        mVortexNormal[v][2] = Math<Real>::SymmetricRandom();
        mVortexNormal[v].Normalize();
        mVortexVariance[v] = Math<Real>::IntervalRandom((Real)0.01,
            (Real)0.1);
        mVortexAmplitude[v] = Math<Real>::IntervalRandom((Real)128,
            (Real)256);
    }

    for (int k = 0; k <= mKMax; ++k)
    {
        Real z = mZ[k];

        for (int j = 0; j <= mJMax; ++j)
        {
            Real y = mY[j];

            for (int i = 0; i <= mIMax; ++i)
            {
                Real x = mX[i];

                // velocity vortex source
                for (v = 0; v < mNumVortices; ++v)
                {
                    Real dx = x - mVortexCenter[v][0];
                    Real dy = y - mVortexCenter[v][1];
                    Real dz = z - mVortexCenter[v][2];
                    Real arg = -(dx*dx + dy*dy + dz*dz)/mVortexVariance[v];

                    Vector3<Real> dir0, dir1;
                    Vector3<Real>::GenerateComplementBasis(dir0, dir1,
                        mVortexNormal[v]);

                    Vector3<Real> vortex = y*dir0 - x*dir1;
                    vortex *= mVortexAmplitude[v]*Math<Real>::Exp(arg);
                    mTimelessVortex[k][j][i][v] = vortex;
                }

                // velocity wind source
                Real sqrLen = x*x + z*z;
                Real ampl = ((Real)64)*Math<Real>::Exp(-sqrLen/(Real)0.01);
                Vector3<Real> wind((Real)0, -ampl, (Real)0);
                mTimelessWind[k][j][i] = wind;
            }
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
Smoke3D<Real>::~Smoke3D ()
{
    delete1(mVortexCenter);
    delete1(mVortexNormal);
    delete1(mVortexVariance);
    delete1(mVortexAmplitude);
    delete4(mTimelessVortex);
    delete3(mTimelessWind);
}
//----------------------------------------------------------------------------
template <typename Real>
Real Smoke3D<Real>::InitialDensity (Real x, Real y, Real z, int, int, int)
{
    Real r = Math<Real>::Sqrt(x*x + y*y + z*z);
    return r <= (Real)0.25 ? (Real)1 : (Real)0;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> Smoke3D<Real>::InitialVelocity (Real, Real, Real, int, int,
    int)
{
    return Vector3<Real>::ZERO;
}
//----------------------------------------------------------------------------
template <typename Real>
Real Smoke3D<Real>::SourceDensity (Real, Real, Real, Real, int, int, int)
{
    return (Real)0;
}
//----------------------------------------------------------------------------
template <typename Real>
Vector3<Real> Smoke3D<Real>::SourceVelocity (Real, Real, Real, Real, int i,
    int j, int k)
{
    Vector3<Real> impulse =
        Vector3<Real>((Real)0, (Real)0, -mGravity) +
        mTimelessWind[k][j][i];

    for (int v = 0; v < mNumActive; ++v)
    {
        impulse += mTimelessVortex[k][j][i][v];
    }

    return impulse;
}
//----------------------------------------------------------------------------
template <typename Real>
void Smoke3D<Real>::SetNumActiveVortices (int numActive)
{
    if (0 <= numActive && numActive <= mNumVortices)
    {
        mNumActive = numActive;
    }
    else
    {
        mNumActive = 0;
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void Smoke3D<Real>::SetGravity (Real gravity)
{
    if (gravity > (Real)0)
    {
        mGravity = gravity;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template class Smoke3D<float>;
template class Smoke3D<double>;
//----------------------------------------------------------------------------
