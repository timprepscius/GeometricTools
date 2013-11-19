// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Smoke2D.h"
#include "Wm5Memory.h"

//----------------------------------------------------------------------------
template <typename Real>
Smoke2D<Real>::Smoke2D (Real x0, Real y0, Real x1, Real y1, Real dt,
    Real denViscosity, Real velViscosity, int imax, int jmax,
    int numGaussSeidelIterations, bool densityDirichlet, int numVortices)
    :
    FLUIDBASE(x0, y0, x1, y1, dt, denViscosity, velViscosity, imax, jmax,
        numGaussSeidelIterations, densityDirichlet),
    mNumVortices(numVortices),
    mNumActive(0),
    mGravity((Real)0)
{
    mVortexCenter = new1<Vector2<Real> >(mNumVortices);
    mVortexVariance = new1<Real>(mNumVortices);
    mVortexAmplitude = new1<Real>(mNumVortices);
    mTimelessDensity = new2<Real>(mIMaxP1, mJMaxP1);
    mTimelessVortex = new3<Vector2<Real> >(mNumVortices, mIMaxP1, mJMaxP1);
    mTimelessWind = new2<Vector2<Real> >(mIMaxP1, mJMaxP1);

    int v;
    for (v = 0; v < mNumVortices; ++v)
    {
        mVortexCenter[v][0] = Math<Real>::UnitRandom();
        mVortexCenter[v][1] = Math<Real>::UnitRandom();
        mVortexVariance[v] = Math<Real>::IntervalRandom((Real)0.001,
            (Real)0.01);
        mVortexAmplitude[v] = Math<Real>::IntervalRandom((Real)128,
            (Real)256);
        if (Math<Real>::SymmetricRandom() < (Real)0)
        {
            mVortexAmplitude[v] *= (Real)-1;
        }
    }

    for (int j = 0; j <= mJMax; ++j)
    {
        Real y = mY[j];

        for (int i = 0; i <= mIMax; ++i)
        {
            Real x = mX[i];

            // density source
            Real dx = x - (Real)0.25;
            Real dy = y - (Real)0.75;
            Real arg = -(dx*dx + dy*dy)/(Real)0.01;
            mTimelessDensity[j][i] = ((Real)2)*Math<Real>::Exp(arg);

            // density sink
            dx = x - (Real)0.75;
            dy = y - (Real)0.25;
            arg = -(dx*dx + dy*dy)/(Real)0.01;
            mTimelessDensity[j][i] -= ((Real)2)*Math<Real>::Exp(arg);

            // velocity vortex source
            for (v = 0; v < mNumVortices; ++v)
            {
                dx = x - mVortexCenter[v][0];
                dy = y - mVortexCenter[v][1];
                arg = -(dx*dx + dy*dy)/mVortexVariance[v];
                Vector2<Real> vortex(dy, -dx);
                vortex *= mVortexAmplitude[v]*Math<Real>::Exp(arg);
                mTimelessVortex[j][i][v] = vortex;
            }

            // velocity wind source
            Real diff = y - (Real)0.5;
            Real ampl = ((Real)32)*Math<Real>::Exp(-diff*diff/(Real)0.001);
            Vector2<Real> wind(ampl, (Real)0);
            mTimelessWind[j][i] = wind;
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
Smoke2D<Real>::~Smoke2D ()
{
    delete1(mVortexCenter);
    delete1(mVortexVariance);
    delete1(mVortexAmplitude);
    delete2(mTimelessDensity);
    delete3(mTimelessVortex);
    delete2(mTimelessWind);
}
//----------------------------------------------------------------------------
template <typename Real>
Real Smoke2D<Real>::InitialDensity (Real, Real, int, int)
{
    return Math<Real>::UnitRandom();
}
//----------------------------------------------------------------------------
template <typename Real>
Vector2<Real> Smoke2D<Real>::InitialVelocity (Real, Real, int, int)
{
    return Vector2<Real>::ZERO;
}
//----------------------------------------------------------------------------
template <typename Real>
Real Smoke2D<Real>::SourceDensity (Real, Real, Real, int i, int j)
{
    return mTimelessDensity[j][i];
}
//----------------------------------------------------------------------------
template <typename Real>
Vector2<Real> Smoke2D<Real>::SourceVelocity (Real, Real, Real, int i, int j)
{
    Vector2<Real> impulse =
        Vector2<Real>((Real)0, -mGravity) +
        mTimelessWind[j][i];

    for (int v = 0; v < mNumActive; ++v)
    {
        impulse += mTimelessVortex[j][i][v];
    }

    return impulse;
}
//----------------------------------------------------------------------------
template <typename Real>
void Smoke2D<Real>::SetNumActiveVortices (int numActive)
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
void Smoke2D<Real>::SetGravity (Real gravity)
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
template class Smoke2D<float>;
template class Smoke2D<double>;
//----------------------------------------------------------------------------
