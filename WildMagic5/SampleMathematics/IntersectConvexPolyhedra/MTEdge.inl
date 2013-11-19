// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline int MTEdge::GetLabel () const
{
    return mLabel;
}
//----------------------------------------------------------------------------
inline int& MTEdge::Label ()
{
    return mLabel;
}
//----------------------------------------------------------------------------
inline int MTEdge::GetVertex (int i) const
{
    return mVertex[i];
}
//----------------------------------------------------------------------------
inline int& MTEdge::Vertex (int i)
{
    return mVertex[i];
}
//----------------------------------------------------------------------------
inline int MTEdge::GetTriangle (int i) const
{
    return mTriangle[i];
}
//----------------------------------------------------------------------------
inline int& MTEdge::Triangle (int i)
{
    return mTriangle[i];
}
//----------------------------------------------------------------------------
