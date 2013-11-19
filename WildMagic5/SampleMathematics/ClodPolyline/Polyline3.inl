// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline int Polyline3::GetNumVertices () const
{
    return mNumVertices;
}
//----------------------------------------------------------------------------
inline const Vector3f* Polyline3::GetVertices () const
{
    return mVertices;
}
//----------------------------------------------------------------------------
inline bool Polyline3::GetClosed () const
{
    return mClosed;
}
//----------------------------------------------------------------------------
inline int Polyline3::GetNumEdges () const
{
    return mNumEdges;
}
//----------------------------------------------------------------------------
inline const int* Polyline3::GetEdges () const
{
    return mEdges;
}
//----------------------------------------------------------------------------
inline int Polyline3::GetMinLevelOfDetail () const
{
    return mVMin;
}
//----------------------------------------------------------------------------
inline int Polyline3::GetMaxLevelOfDetail () const
{
    return mVMax;
}
//----------------------------------------------------------------------------
inline int Polyline3::GetLevelOfDetail () const
{
    return mNumVertices;
}
//----------------------------------------------------------------------------
