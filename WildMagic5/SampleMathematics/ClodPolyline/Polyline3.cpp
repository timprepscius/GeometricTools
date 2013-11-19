// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Polyline3.h"
#include "VertexCollapse.h"
#include "Wm5Memory.h"

//----------------------------------------------------------------------------
Polyline3::Polyline3 (int numVertices, Vector3f* vertices, bool closed)
    :
    mNumVertices(numVertices),
    mVertices(vertices),
    mClosed(closed)
{
    assertion(vertices && (closed ? numVertices >= 3 : numVertices >= 2),
        "Invalid inputs.\n");

    // Compute the vertex collapses.
    VertexCollapse(mNumVertices, mVertices, mClosed, mIndexMap, mNumEdges,
        mEdges);

    // Polyline initially at full level of detail.
    mVMin = (mClosed ? 3 : 2);
    mVMax = mNumVertices;
}
//----------------------------------------------------------------------------
Polyline3::~Polyline3 ()
{
    delete1(mVertices);
    delete1(mEdges);
    delete1(mIndexMap);
}
//----------------------------------------------------------------------------
void Polyline3::SetLevelOfDetail (int numVertices)
{
    if (numVertices < mVMin || numVertices > mVMax)
    {
        return;
    }

    // Decrease level of detail.
    while (mNumVertices > numVertices)
    {
        --mNumVertices;
        mEdges[mIndexMap[mNumVertices]] = mEdges[2*mNumEdges - 1];
        --mNumEdges;
    }

    // Increase level of detail.
    while (mNumVertices < numVertices)
    {
        ++mNumEdges;
        mEdges[mIndexMap[mNumVertices]] = mNumVertices;
        ++mNumVertices;
    }
}
//----------------------------------------------------------------------------
