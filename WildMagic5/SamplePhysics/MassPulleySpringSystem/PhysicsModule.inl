// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline double PhysicsModule::GetInitialY1 () const
{
    return mY1;
}
//----------------------------------------------------------------------------
inline double PhysicsModule::GetCurrentY1 () const
{
    return mY1Curr;
}
//----------------------------------------------------------------------------
inline double PhysicsModule::GetCurrentY2 () const
{
    return mY2Curr;
}
//----------------------------------------------------------------------------
inline double PhysicsModule::GetCurrentY3 () const
{
    return mY3Curr;
}
//----------------------------------------------------------------------------
inline double PhysicsModule::GetAngle () const
{
    return (mY1 - mY1Curr)/Radius;
}
//----------------------------------------------------------------------------
inline double PhysicsModule::GetCableFraction1 () const
{
    return (mY1Curr - mY3Curr)/WireLength;
}
//----------------------------------------------------------------------------
inline double PhysicsModule::GetCableFraction2 () const
{
    return (mY2Curr - mY3Curr)/WireLength;
}
//----------------------------------------------------------------------------
