// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
inline int Smoke2D<Real>::GetNumVortices () const
{
    return mNumVortices;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector2<Real> Smoke2D<Real>::GetVortexCenter (int i) const
{
    return mVortexCenter[i];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Smoke2D<Real>::GetVortexVariance (int i) const
{
    return mVortexVariance[i];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Smoke2D<Real>::GetVortexAmplitude (int i) const
{
    return mVortexAmplitude[i];
}
//----------------------------------------------------------------------------
template <typename Real>
inline int Smoke2D<Real>::GetNumActiveVortices () const
{
    return mNumActive;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Smoke2D<Real>::GetGravity () const
{
    return mGravity;
}
//----------------------------------------------------------------------------
