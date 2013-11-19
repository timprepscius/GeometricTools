// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2012/07/07)

#ifndef PARTITIONMESH_H
#define PARTITIONMESH_H

#include "Wm5EdgeKey.h"
#include "Wm5HPlane.h"
using namespace Wm5;

class PartitionMesh
{
public:
    PartitionMesh (const std::vector<APoint>& vertices,
        const std::vector<int>& indices, const HPlane& plane,
        std::vector<APoint>& clipVertices, std::vector<int>& negIndices,
        std::vector<int>& posIndices);

private:
    void ClassifyVertices (const std::vector<APoint>& clipVertices,
        const HPlane& plane);

    void ClassifyEdges (std::vector<APoint>& clipVertices,
        const std::vector<int>& indices);

    void ClassifyTriangles (const std::vector<int>& indices,
        std::vector<int>& negIndices, std::vector<int>& posIndices);

    void AppendTriangle (std::vector<int>& indices, int v0, int v1, int v2);

    void SplitTrianglePPM (std::vector<int>& negIndices,
        std::vector<int>& posIndices, int v0, int v1, int v2);

    void SplitTriangleMMP (std::vector<int>& rkNegIndices,
        std::vector<int>& rkPosIndices, int v0, int v1, int v2);

    void SplitTrianglePMZ (std::vector<int>& rkNegIndices,
        std::vector<int>& rkPosIndices, int v0, int v1, int v2);

    void SplitTriangleMPZ (std::vector<int>& rkNegIndices,
        std::vector<int>& rkPosIndices, int v0, int v1, int v2);

    // Stores the signed distances from the vertices to the plane.
    std::vector<float> mSignedDistances;

    // Stores the edges whose vertices are on opposite sides of the
    // plane.  The key is a pair of indices into the vertex array.
    // The value is the point of intersection of the edge with the
    // plane and an index into m_kVertices (the index is larger or
    // equal to the number of vertices of incoming rkVertices).
    std::map<EdgeKey,std::pair<APoint, int> > mEMap;
};

#endif
