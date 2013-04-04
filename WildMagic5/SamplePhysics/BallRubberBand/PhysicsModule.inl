// Geometric Tools, LLC
// Copyright (c) 1998-2012
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
inline const Vector2d& PhysicsModule::GetPosition () const
{
    return mPosition;
}
//----------------------------------------------------------------------------
inline const Vector2d& PhysicsModule::GetVelocity () const
{
    return mVelocity;
}
//----------------------------------------------------------------------------
inline double PhysicsModule::GetFrequency () const
{
    return mFrequency;
}
//----------------------------------------------------------------------------
