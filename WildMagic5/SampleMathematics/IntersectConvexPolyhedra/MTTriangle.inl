// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline int MTTriangle::GetLabel () const
{
    return mLabel;
}
//----------------------------------------------------------------------------
inline int& MTTriangle::Label ()
{
    return mLabel;
}
//----------------------------------------------------------------------------
inline int MTTriangle::GetVertex (int i) const
{
    return mVertex[i];
}
//----------------------------------------------------------------------------
inline int& MTTriangle::Vertex (int i)
{
    return mVertex[i];
}
//----------------------------------------------------------------------------
inline int MTTriangle::GetEdge (int i) const
{
    return mEdge[i];
}
//----------------------------------------------------------------------------
inline int& MTTriangle::Edge (int i)
{
    return mEdge[i];
}
//----------------------------------------------------------------------------
inline int MTTriangle::GetAdjacent (int i) const
{
    return mAdjacent[i];
}
//----------------------------------------------------------------------------
inline int& MTTriangle::Adjacent (int i)
{
    return mAdjacent[i];
}
//----------------------------------------------------------------------------
