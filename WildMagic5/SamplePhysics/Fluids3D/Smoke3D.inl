// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
template <typename Real>
inline int Smoke3D<Real>::GetNumVortices () const
{
    return mNumVortices;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector3<Real> Smoke3D<Real>::GetVortexCenter (int i) const
{
    return mVortexCenter[i];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Vector3<Real> Smoke3D<Real>::GetVortexNormal (int i) const
{
    return mVortexNormal[i];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Smoke3D<Real>::GetVortexVariance (int i) const
{
    return mVortexVariance[i];
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Smoke3D<Real>::GetVortexAmplitude (int i) const
{
    return mVortexAmplitude[i];
}
//----------------------------------------------------------------------------
template <typename Real>
inline int Smoke3D<Real>::GetNumActiveVortices () const
{
    return mNumActive;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Smoke3D<Real>::GetGravity () const
{
    return mGravity;
}
//----------------------------------------------------------------------------
