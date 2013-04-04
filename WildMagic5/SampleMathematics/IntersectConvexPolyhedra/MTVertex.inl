// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline int MTVertex::GetLabel () const
{
    return mLabel;
}
//----------------------------------------------------------------------------
inline int MTVertex::GetNumEdges () const
{
    return mESet.GetNumElements();
}
//----------------------------------------------------------------------------
inline int MTVertex::GetEdge (int e) const
{
    return mESet.Get(e);
}
//----------------------------------------------------------------------------
inline bool MTVertex::InsertEdge (int e)
{
    return mESet.Insert(e);
}
//----------------------------------------------------------------------------
inline bool MTVertex::RemoveEdge (int e)
{
    return mESet.Remove(e);
}
//----------------------------------------------------------------------------
inline int MTVertex::GetNumTriangles () const
{
    return mTSet.GetNumElements();
}
//----------------------------------------------------------------------------
inline int MTVertex::GetTriangle (int i) const
{
    return mTSet.Get(i);
}
//----------------------------------------------------------------------------
inline bool MTVertex::InsertTriangle (int t)
{
    return mTSet.Insert(t);
}
//----------------------------------------------------------------------------
inline bool MTVertex::RemoveTriangle (int t)
{
    return mTSet.Remove(t);
}
//----------------------------------------------------------------------------
inline bool MTVertex::operator== (const MTVertex& vertex) const
{
    return mLabel == vertex.mLabel;
}
//----------------------------------------------------------------------------
