// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2012/07/06)

#include "PartitionMesh.h"

//----------------------------------------------------------------------------
PartitionMesh::PartitionMesh (const std::vector<APoint>& vertices,
    const std::vector<int>& indices, const HPlane& plane,
    std::vector<APoint>& clipVertices, std::vector<int>& negIndices,
    std::vector<int>& posIndices)
    :
    mSignedDistances(vertices.size())
{
    // Make a copy of the incoming vertices.  If the mesh intersects the
    // plane, new vertices must be generated.  These are appended to the
    // clipVertices array.
    clipVertices = vertices;

    ClassifyVertices(clipVertices, plane);
    ClassifyEdges(clipVertices, indices);
    ClassifyTriangles(indices, negIndices, posIndices);
}
//----------------------------------------------------------------------------
void PartitionMesh::ClassifyVertices (
    const std::vector<APoint>& clipVertices, const HPlane& plane)
{
    const int numVertices = (int)clipVertices.size();
    for (int i = 0; i < numVertices; ++i)
    {
        mSignedDistances[i] = plane.DistanceTo(clipVertices[i]);
    }
}
//----------------------------------------------------------------------------
void PartitionMesh::ClassifyEdges (std::vector<APoint>& clipVertices,
    const std::vector<int>& indices)
{
    const int numTriangles = (int)indices.size()/3;
    int nextIndex = (int)clipVertices.size();
    for (int i = 0; i < numTriangles; ++i)
    {
        int v0 = indices[3*i+0];
        int v1 = indices[3*i+1];
        int v2 = indices[3*i+2];
        float sDist0 = mSignedDistances[v0];
        float sDist1 = mSignedDistances[v1];
        float sDist2 = mSignedDistances[v2];

        EdgeKey key;
        float t;
        APoint intr;
        AVector diff;

        // The change-in-sign tests are structured this way to avoid numerical
        // round-off problems.  For example, sDist0 > 0 and sDist1 < 0, but
        // both are very small and sDist0*sDist1 = 0 due to round-off
        // errors.  The tests also guarantee consistency between this function
        // and ClassifyTriangles, the latter function using sign tests only on
        // the individual sDist values.

        if ((sDist0 > 0.0f && sDist1 < 0.0f)
        ||  (sDist0 < 0.0f && sDist1 > 0.0f))
        {
            key = EdgeKey(v0, v1);
            if (mEMap.find(key) == mEMap.end())
            {
                t = sDist0/(sDist0 - sDist1);
                diff = clipVertices[v1] - clipVertices[v0];
                intr = clipVertices[v0] + t*diff;
                clipVertices.push_back(intr);
                mEMap[key] = std::make_pair(intr, nextIndex);
                ++nextIndex;
            }
        }

        if ((sDist1 > 0.0f && sDist2 < 0.0f)
        ||  (sDist1 < 0.0f && sDist2 > 0.0f))
        {
            key = EdgeKey(v1,v2);
            if (mEMap.find(key) == mEMap.end())
            {
                t = sDist1/(sDist1 - sDist2);
                diff = clipVertices[v2] - clipVertices[v1];
                intr = clipVertices[v1] + t*diff;
                clipVertices.push_back(intr);
                mEMap[key] = std::make_pair(intr, nextIndex);
                ++nextIndex;
            }
        }

        if ((sDist2 > 0.0f && sDist0 < 0.0f)
        ||  (sDist2 < 0.0f && sDist0 > 0.0f))
        {
            key = EdgeKey(v2,v0);
            if (mEMap.find(key) == mEMap.end())
            {
                t = sDist2/(sDist2 - sDist0);
                diff = clipVertices[v0] - clipVertices[v2];
                intr = clipVertices[v2] + t*diff;
                clipVertices.push_back(intr);
                mEMap[key] = std::make_pair(intr, nextIndex);
                ++nextIndex;
            }
        }
    }
}
//----------------------------------------------------------------------------
void PartitionMesh::ClassifyTriangles (const std::vector<int>& indices,
    std::vector<int>& negIndices, std::vector<int>& posIndices)
{
    const int numTriangles = (int)indices.size()/3;
    for (int i = 0; i < numTriangles; ++i)
    {
        int v0 = indices[3*i+0];
        int v1 = indices[3*i+1];
        int v2 = indices[3*i+2];
        float sDist0 = mSignedDistances[v0];
        float sDist1 = mSignedDistances[v1];
        float sDist2 = mSignedDistances[v2];

        if (sDist0 > 0.0f)
        {
            if (sDist1 > 0.0f)
            {
                if (sDist2 > 0.0f)
                {
                    // +++
                    AppendTriangle(posIndices, v0, v1, v2);
                }
                else if (sDist2 < 0.0f)
                {
                    // ++-
                    SplitTrianglePPM(negIndices, posIndices, v0, v1, v2);
                }
                else
                {
                    // ++0
                    AppendTriangle(posIndices, v0, v1, v2);
                }
            }
            else if (sDist1 < 0.0f)
            {
                if (sDist2 > 0.0f)
                {
                    // +-+
                    SplitTrianglePPM(negIndices, posIndices, v2, v0, v1);
                }
                else if (sDist2 < 0.0f)
                {
                    // +--
                    SplitTriangleMMP(negIndices, posIndices, v1, v2, v0);
                }
                else
                {
                    // +-0
                    SplitTrianglePMZ(negIndices, posIndices, v0, v1, v2);
                }
            }
            else
            {
                if (sDist2 > 0.0f)
                {
                    // +0+
                    AppendTriangle(posIndices, v0, v1, v2);
                }
                else if (sDist2 < 0.0f)
                {
                    // +0-
                    SplitTriangleMPZ(negIndices, posIndices, v2, v0, v1);
                }
                else
                {
                    // +00
                    AppendTriangle(posIndices, v0, v1, v2);
                }
            }
        }
        else if (sDist0 < 0.0f)
        {
            if (sDist1 > 0.0f)
            {
                if (sDist2 > 0.0f)
                {
                    // -++
                    SplitTrianglePPM(negIndices, posIndices, v1, v2, v0);
                }
                else if (sDist2 < 0.0f)
                {
                    // -+-
                    SplitTriangleMMP(negIndices, posIndices, v2, v0, v1);
                }
                else
                {
                    // -+0
                    SplitTriangleMPZ(negIndices, posIndices, v0, v1, v2);
                }
            }
            else if (sDist1 < 0.0f)
            {
                if (sDist2 > 0.0f)
                {
                    // --+
                    SplitTriangleMMP(negIndices, posIndices, v0, v1, v2);
                }
                else if (sDist2 < 0.0f)
                {
                    // ---
                    AppendTriangle(negIndices, v0, v1, v2);
                }
                else
                {
                    // --0
                    AppendTriangle(negIndices, v0, v1, v2);
                }
            }
            else
            {
                if (sDist2 > 0.0f)
                {
                    // -0+
                    SplitTrianglePMZ(negIndices, posIndices, v2, v0, v1);
                }
                else if (sDist2 < 0.0f)
                {
                    // -0-
                    AppendTriangle(negIndices, v0, v1, v2);
                }
                else
                {
                    // -00
                    AppendTriangle(negIndices, v0, v1, v2);
                }
            }
        }
        else
        {
            if (sDist1 > 0.0f)
            {
                if (sDist2 > 0.0f)
                {
                    // 0++
                    AppendTriangle(posIndices, v0, v1, v2);
                }
                else if (sDist2 < 0.0f)
                {
                    // 0+-
                    SplitTrianglePMZ(negIndices, posIndices, v1, v2, v0);
                }
                else
                {
                    // 0+0
                    AppendTriangle(posIndices, v0, v1, v2);
                }
            }
            else if (sDist1 < 0.0f)
            {
                if (sDist2 > 0.0f)
                {
                    // 0-+
                    SplitTriangleMPZ(negIndices, posIndices, v1, v2, v0);
                }
                else if (sDist2 < 0.0f)
                {
                    // 0--
                    AppendTriangle(negIndices, v0, v1, v2);
                }
                else
                {
                    // 0-0
                    AppendTriangle(negIndices, v0, v1, v2);
                }
            }
            else
            {
                if (sDist2 > 0.0f)
                {
                    // 00+
                    AppendTriangle(posIndices, v0, v1, v2);
                }
                else if (sDist2 < 0.0f)
                {
                    // 00-
                    AppendTriangle(negIndices, v0, v1, v2);
                }
                else
                {
                    // 000, reject triangles lying in the plane
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
void PartitionMesh::AppendTriangle (std::vector<int>& indices, int v0, int v1,
    int v2)
{
    indices.push_back(v0);
    indices.push_back(v1);
    indices.push_back(v2);
}
//----------------------------------------------------------------------------
void PartitionMesh::SplitTrianglePPM (std::vector<int>& negIndices,
    std::vector<int>& posIndices, int v0, int v1, int v2)
{
    int v12 = mEMap[EdgeKey(v1, v2)].second;
    int v20 = mEMap[EdgeKey(v2, v0)].second;
    posIndices.push_back(v0);
    posIndices.push_back(v1);
    posIndices.push_back(v12);
    posIndices.push_back(v0);
    posIndices.push_back(v12);
    posIndices.push_back(v20);
    negIndices.push_back(v2);
    negIndices.push_back(v20);
    negIndices.push_back(v12);
}
//----------------------------------------------------------------------------
void PartitionMesh::SplitTriangleMMP (std::vector<int>& negIndices,
    std::vector<int>& posIndices, int v0, int v1, int v2)
{
    int v12 = mEMap[EdgeKey(v1, v2)].second;
    int v20 = mEMap[EdgeKey(v2, v0)].second;
    negIndices.push_back(v0);
    negIndices.push_back(v1);
    negIndices.push_back(v12);
    negIndices.push_back(v0);
    negIndices.push_back(v12);
    negIndices.push_back(v20);
    posIndices.push_back(v2);
    posIndices.push_back(v20);
    posIndices.push_back(v12);
}
//----------------------------------------------------------------------------
void PartitionMesh::SplitTrianglePMZ (std::vector<int>& negIndices,
    std::vector<int>& posIndices, int v0, int v1, int v2)
{
    int v01 = mEMap[EdgeKey(v0, v1)].second;
    posIndices.push_back(v2);
    posIndices.push_back(v0);
    posIndices.push_back(v01);
    negIndices.push_back(v2);
    negIndices.push_back(v01);
    negIndices.push_back(v1);
}
//----------------------------------------------------------------------------
void PartitionMesh::SplitTriangleMPZ (std::vector<int>& negIndices,
    std::vector<int>& posIndices, int v0, int v1, int v2)
{
    int v01 = mEMap[EdgeKey(v0, v1)].second;
    negIndices.push_back(v2);
    negIndices.push_back(v0);
    negIndices.push_back(v01);
    posIndices.push_back(v2);
    posIndices.push_back(v01);
    posIndices.push_back(v1);
}
//----------------------------------------------------------------------------
