// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline double PhysicsModule::GetTime () const
{
    return mTime;
}
//----------------------------------------------------------------------------
inline double PhysicsModule::GetDeltaTime () const
{
    return mDeltaTime;
}
//----------------------------------------------------------------------------
inline double PhysicsModule::GetTheta () const
{
    return mState[0];
}
//----------------------------------------------------------------------------
inline double PhysicsModule::GetThetaDot () const
{
    return mState[1];
}
//----------------------------------------------------------------------------
inline double PhysicsModule::GetRadius () const
{
    return mState[2];
}
//----------------------------------------------------------------------------
inline double PhysicsModule::GetRadiusDot () const
{
    return mState[3];
}
//----------------------------------------------------------------------------
inline double PhysicsModule::GetEccentricity () const
{
    return mEccentricity;
}
//----------------------------------------------------------------------------
inline double PhysicsModule::GetMajorAxis () const
{
    return mMajorAxis;
}
//----------------------------------------------------------------------------
inline double PhysicsModule::GetMinorAxis () const
{
    return mMinorAxis;
}
//----------------------------------------------------------------------------
