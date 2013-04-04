// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline float Smoke2D::GetX0 () const
{
    return mX0;
}
//----------------------------------------------------------------------------
inline float Smoke2D::GetY0 () const
{
    return mY0;
}
//----------------------------------------------------------------------------
inline float Smoke2D::GetX1 () const
{
    return mX1;
}
//----------------------------------------------------------------------------
inline float Smoke2D::GetY1 () const
{
    return mY1;
}
//----------------------------------------------------------------------------
inline float Smoke2D::GetDt () const
{
    return mDt;
}
//----------------------------------------------------------------------------
inline float Smoke2D::GetDx () const
{
    return mDx;
}
//----------------------------------------------------------------------------
inline float Smoke2D::GetDy () const
{
    return mDy;
}
//----------------------------------------------------------------------------
inline float Smoke2D::GetTime () const
{
    return mTime;
}
//----------------------------------------------------------------------------
inline int Smoke2D::GetIMax () const
{
    return mIMax;
}
//----------------------------------------------------------------------------
inline int Smoke2D::GetJMax () const
{
    return mJMax;
}
//----------------------------------------------------------------------------
inline int Smoke2D::GetNumVortices () const
{
    return mNumVortices;
}
//----------------------------------------------------------------------------
inline Vector2f Smoke2D::GetVortexCenter (int i) const
{
    return mVortexCenter[i];
}
//----------------------------------------------------------------------------
inline float Smoke2D::GetVortexVariance (int i) const
{
    return mVortexVariance[i];
}
//----------------------------------------------------------------------------
inline float Smoke2D::GetVortexAmplitude (int i) const
{
    return mVortexAmplitude[i];
}
//----------------------------------------------------------------------------
inline int Smoke2D::GetNumActiveVortices () const
{
    return mNumActive;
}
//----------------------------------------------------------------------------
inline float Smoke2D::GetGravity () const
{
    return -mGravity[1];
}
//----------------------------------------------------------------------------
