// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline TriMesh* DeformableBall::GetMesh () const
{
    return mMesh;
}
//----------------------------------------------------------------------------
inline float DeformableBall::GetDuration () const
{
    return mDuration;
}
//----------------------------------------------------------------------------
inline float DeformableBall::GetPeriod () const
{
    return mPeriod;
}
//----------------------------------------------------------------------------
inline float DeformableBall::GetMinActive () const
{
    return mMinActive;
}
//----------------------------------------------------------------------------
inline float DeformableBall::GetMaxActive () const
{
    return mMaxActive;
}
//----------------------------------------------------------------------------
inline float DeformableBall::GetAmplitude (float time)
{
    return mDeformMult*(time - mMinActive)*(mMaxActive - time);
}
//----------------------------------------------------------------------------
inline bool& DeformableBall::DoAffine ()
{
    return mDoAffine;
}
//----------------------------------------------------------------------------
