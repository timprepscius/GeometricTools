// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

//----------------------------------------------------------------------------
inline int MTMesh::GetNumVertices () const
{
    return mVertices.GetNumElements();
}
//----------------------------------------------------------------------------
inline int MTMesh::V (int label) const
{
    VCIter iter = mVMap.find(MTIVertex(label));
    return (iter != mVMap.end() ? iter->second : -1);
}
//----------------------------------------------------------------------------
inline const MTVertex& MTMesh::GetVertex (int vIndex) const
{
    return mVertices.Get(vIndex);
}
//----------------------------------------------------------------------------
inline int MTMesh::GetVLabel (int vIndex) const
{
    return mVertices.Get(vIndex).GetLabel();
}
//----------------------------------------------------------------------------
inline int MTMesh::GetNumEdges () const
{
    return mEdges.GetNumElements();
}
//----------------------------------------------------------------------------
inline int MTMesh::E (int label0, int label1) const
{
    ECIter iter = mEMap.find(MTIEdge(label0, label1));
    return (iter != mEMap.end() ? iter->second : -1);
}
//----------------------------------------------------------------------------
inline const MTEdge& MTMesh::GetEdge (int eIndex) const
{
    return mEdges.Get(eIndex);
}
//----------------------------------------------------------------------------
inline int MTMesh::GetELabel (int eIndex) const
{
    return mEdges.Get(eIndex).GetLabel();
}
//----------------------------------------------------------------------------
inline int& MTMesh::ELabel (int eIndex)
{
    return mEdges[eIndex].Label();
}
//----------------------------------------------------------------------------
inline int MTMesh::GetNumTriangles () const
{
    return mTriangles.GetNumElements();
}
//----------------------------------------------------------------------------
inline const MTTriangle& MTMesh::GetTriangle (int tIndex) const
{
    return mTriangles.Get(tIndex);
}
//----------------------------------------------------------------------------
inline int MTMesh::T (int label0, int label1, int label2) const
{
    TCIter iter = mTMap.find(MTITriangle(label0, label1, label2));
    return (iter != mTMap.end() ? iter->second : -1);
}
//----------------------------------------------------------------------------
inline int MTMesh::GetTLabel (int tIndex) const
{
    return mTriangles.Get(tIndex).GetLabel();
}
//----------------------------------------------------------------------------
inline int& MTMesh::TLabel (int tIndex)
{
    return mTriangles[tIndex].Label();
}
//----------------------------------------------------------------------------
inline int& MTMesh::InitialELabel ()
{
    return mInitialELabel;
}
//----------------------------------------------------------------------------
inline int& MTMesh::InitialTLabel ()
{
    return mInitialTLabel;
}
//----------------------------------------------------------------------------
