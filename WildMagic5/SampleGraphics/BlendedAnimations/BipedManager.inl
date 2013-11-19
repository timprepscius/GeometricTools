// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.1.0 (2010/04/14)

//----------------------------------------------------------------------------
inline Node* BipedManager::GetRoot ()
{
    return mRoot;
}
//----------------------------------------------------------------------------
inline void BipedManager::GetIdle (double& minTime, double& maxTime) const
{
    GetAnimation(mIdleArray, minTime, maxTime);
}
//----------------------------------------------------------------------------
inline void BipedManager::GetWalk (double& minTime, double& maxTime) const
{
    GetAnimation(mWalkArray, minTime, maxTime);
}
//----------------------------------------------------------------------------
inline void BipedManager::GetRun (double& minTime, double& maxTime) const
{
    GetAnimation(mRunArray, minTime, maxTime);
}
//----------------------------------------------------------------------------
inline void BipedManager::SetIdle (double frequency, double phase)
{
    SetAnimation(mIdleArray, frequency, phase);
}
//----------------------------------------------------------------------------
inline void BipedManager::SetWalk (double frequency, double phase)
{
    SetAnimation(mWalkArray, frequency, phase);
}
//----------------------------------------------------------------------------
inline void BipedManager::SetRun (double frequency, double phase)
{
    SetAnimation(mRunArray, frequency, phase);
}
//----------------------------------------------------------------------------
inline void BipedManager::DoIdle ()
{
    DoAnimation(mIdleArray);
}
//----------------------------------------------------------------------------
inline void BipedManager::DoWalk ()
{
    DoAnimation(mWalkArray);
}
//----------------------------------------------------------------------------
inline void BipedManager::DoRun ()
{
    DoAnimation(mRunArray);
}
//----------------------------------------------------------------------------
inline void BipedManager::DoIdleWalk ()
{
    DoAnimation(mIdleWalkArray);
}
//----------------------------------------------------------------------------
inline void BipedManager::DoWalkRun ()
{
    DoAnimation(mWalkRunArray);
}
//----------------------------------------------------------------------------
inline void BipedManager::SetIdleWalk (float weight)
{
    SetBlendAnimation(mIdleWalkArray, weight);
}
//----------------------------------------------------------------------------
inline void BipedManager::SetWalkRun (float weight)
{
    SetBlendAnimation(mWalkRunArray, weight);
}
//----------------------------------------------------------------------------
