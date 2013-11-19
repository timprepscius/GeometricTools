// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.5.2 (2012/07/06)

#include "Climb3D.h"
#include "VETable.h"
#include "Wm5Memory.h"

//----------------------------------------------------------------------------
Climb3D::Climb3D (int N, float* data, bool fixBoundary)
    :
    mN(N),
    mTwoPowerN(1 << N),
    mSize(mTwoPowerN + 1),
    mSizeSqr(mSize*mSize),
    mData(data),
    mLevel(0.0f)
{
    assertion(0 <= N && N < 32 && data, "Invalid input data.\n");

    // This is 'false' by default.  If set to 'true', image boundary voxels
    // are not allowed to merge with any other voxels.  This forces highest
    // level of detail on the boundary.  The idea is that an image too large
    // to process by itself can be partitioned into smaller subimages and the
    // adaptive skeleton climbing applied to each subimage.  By forcing
    // highest resolution on the boundary, adjacent subimages will not have
    // any cracking problems.
    mFixBoundary = fixBoundary;

    // Linear merging.
    mXMerge = new1<LinearMergeTree**>(mSize);
    mYMerge = new1<LinearMergeTree**>(mSize);
    mZMerge = new1<LinearMergeTree**>(mSize);
    for (int i = 0; i < mSize; ++i)
    {
        mXMerge[i] = new1<LinearMergeTree*>(mSize);
        mYMerge[i] = new1<LinearMergeTree*>(mSize);
        mZMerge[i] = new1<LinearMergeTree*>(mSize);
        for (int j = 0; j < mSize; ++j)
        {
            mXMerge[i][j] = new0 LinearMergeTree(mN);
            mYMerge[i][j] = new0 LinearMergeTree(mN);
            mZMerge[i][j] = new0 LinearMergeTree(mN);
        }
    }
}
//----------------------------------------------------------------------------
Climb3D::~Climb3D ()
{
    delete1(mData);

    for (int i = 0; i < mSize; ++i)
    {
        for (int j = 0; j < mSize; ++j)
        {
            delete0(mXMerge[i][j]);
            delete0(mYMerge[i][j]);
            delete0(mZMerge[i][j]);
        }
        delete1(mXMerge[i]);
        delete1(mYMerge[i]);
        delete1(mZMerge[i]);
    }
    delete1(mXMerge);
    delete1(mYMerge);
    delete1(mZMerge);
}
//----------------------------------------------------------------------------
void Climb3D::ExtractContour (float level, int depth, int& numVertices,
    Vector3f*& vertices, int& numTriangles, TriangleKey*& triangles)
{
    std::vector<Vector3f> localVertices;
    std::vector<TriangleKey> localTriangles;
    mBoxes.clear();

    mLevel = level;
    Merge(depth);
    Tessellate(localVertices, localTriangles);

    numVertices = (int)localVertices.size();
    vertices = new1<Vector3f>(numVertices);
    memcpy(vertices, &localVertices[0], numVertices*sizeof(Vector3f));

    numTriangles = (int)localTriangles.size();
    triangles = new1<TriangleKey>(numTriangles);
    memcpy(triangles, &localTriangles[0], numTriangles*sizeof(TriangleKey));
}
//----------------------------------------------------------------------------
void Climb3D::MakeUnique (int& numVertices, Vector3f*& vertices,
    int& numTriangles, TriangleKey*& triangles)
{
    if (numVertices == 0)
    {
        return;
    }

    // Use a hash table to generate unique storage.
    typedef std::map<Vector3f,int> VMap;
    typedef VMap::iterator VIterator;
    VMap vMap;
    for (int v = 0, nextVertex = 0; v < numVertices; ++v)
    {
        std::pair<VIterator,bool> result = vMap.insert(
            std::make_pair(vertices[v], nextVertex));
        if (result.second == true)
        {
            ++nextVertex;
        }
    }

    // Use a hash table to generate unique storage.
    typedef std::map<TriangleKey,int> TMap;
    typedef TMap::iterator TIterator;
    TMap* tMap = NULL;
    int t;
    VIterator viter;

    if (numTriangles)
    {
        tMap = new0 TMap();
        int nextTriangle = 0;
        for (t = 0; t < numTriangles; t++)
        {
            // Replace old vertex indices by new ones.
            viter = vMap.find(vertices[triangles[t].V[0]]);
            assertion(viter != vMap.end(), "Unexpected condition.\n");
            triangles[t].V[0] = viter->second;
            viter = vMap.find(vertices[triangles[t].V[1]]);
            assertion(viter != vMap.end(), "Unexpected condition.\n");
            triangles[t].V[1] = viter->second;
            viter = vMap.find(vertices[triangles[t].V[2]]);
            assertion(viter != vMap.end(), "Unexpected condition.\n");
            triangles[t].V[2] = viter->second;

            // Keep only unique triangles.
            std::pair<TIterator,bool> result = tMap->insert(
                std::make_pair(triangles[t], nextTriangle));
            if (result.second == true)
            {
                ++nextTriangle;
            }
        }
    }

    // Pack vertices into an array.
    delete1(vertices);
    if (numVertices > 0)
    {
        numVertices = (int)vMap.size();
        vertices = new1<Vector3f>(numVertices);
        for (viter = vMap.begin(); viter != vMap.end(); ++viter)
        {
            vertices[viter->second] = viter->first;
        }
    }
    else
    {
        vertices = 0;
    }

    // Pack triangles into an array.
    delete1(triangles);
    if (numTriangles > 0)
    {
        numTriangles = (int)tMap->size();
        triangles = new1<TriangleKey>(numTriangles);
        TIterator titer;
        for (titer = tMap->begin(); titer != tMap->end(); ++titer)
        {
            triangles[titer->second] = titer->first;
        }
        delete0(tMap);
    }
    else
    {
        triangles = 0;
    }
}
//----------------------------------------------------------------------------
void Climb3D::OrientTriangles (const Vector3f* positions, int numTriangles,
    TriangleKey* triangles, bool sameDirection)
{
    const float oneThird = 1.0f/3.0f;
    for (int i = 0; i < numTriangles; ++i)
    {
        TriangleKey& triangle = triangles[i];

        // Get triangle vertices.
        const Vector3f& pos0 = *(Vector3f*)&positions[triangle.V[0]];
        const Vector3f& pos1 = *(Vector3f*)&positions[triangle.V[1]];
        const Vector3f& pos2 = *(Vector3f*)&positions[triangle.V[2]];
        
        // Construct a triangle normal based on current orientation.
        Vector3f edge1 = pos1 - pos0;
        Vector3f edge2 = pos2 - pos0;
        Vector3f normal = edge1.Cross(edge2);

        // Compute the average image gradient at the vertices.
        Vector3f grad0 = GetGradient(pos0);
        Vector3f grad1 = GetGradient(pos1);
        Vector3f grad2 = GetGradient(pos2);
        Vector3f avrGrad = oneThird*(grad0 + grad1 + grad2);
        
        // Compute the dot product of normal and average gradient.
        float dot = normal.Dot(avrGrad);

        // Choose triangle orientation based on gradient direction.
        int save;
        if (sameDirection)
        {
            if (dot < 0.0f)
            {
                // Wrong orientation, reorder it.
                save = triangle.V[1];
                triangle.V[1] = triangle.V[2];
                triangle.V[2] = save;
            }
        }
        else
        {
            if (dot > 0.0f)
            {
                // Wrong orientation, reorder it.
                save = triangle.V[1];
                triangle.V[1] = triangle.V[2];
                triangle.V[2] = save;
            }
        }
    }
}
//----------------------------------------------------------------------------
Vector3f* Climb3D::ComputeNormals (int numVertices,
    const Vector3f* positions, int numTriangles,
    const TriangleKey* triangles)
{
    // Maintain a running sum of triangle normals at each vertex.
    Vector3f* normals = new1<Vector3f>(numVertices);
    memset(normals, 0, numVertices*sizeof(Vector3f));
    int i, j;
    for (i = 0, j = 0; i < numTriangles; ++i)
    {
        const TriangleKey& triangle = triangles[i];
        const Vector3f& pos0 = positions[triangle.V[0]];
        const Vector3f& pos1 = positions[triangle.V[1]];
        const Vector3f& pos2 = positions[triangle.V[2]];
        
        // Construct triangle normal.
        Vector3f edge1 = pos1 - pos0;
        Vector3f edge2 = pos2 - pos0;
        Vector3f normal = edge1.Cross(edge2);
        
        // Maintain the sum of normals at each vertex.
        normals[triangle.V[0]] += normal;
        normals[triangle.V[1]] += normal;
        normals[triangle.V[2]] += normal;
    }

    // The normal vector storage was used to accumulate the sum of triangle
    // normals.  Now these vectors must be rescaled to be unit length.
    for (i = 0; i < numVertices; ++i)
    {
        normals[i].Normalize();
    }
    
    return normals;
}
//----------------------------------------------------------------------------
void Climb3D::PrintBoxes (const char* filename)
{
    std::ofstream outFile(filename);
    const int numBoxes = (int)mBoxes.size();
    outFile << numBoxes << std::endl;
    for (int i = 0; i < numBoxes; ++i)
    {
        OctBox& box = mBoxes[i];
        outFile << "box " << i << ": ";
        outFile << "x0 = " << box.mX0 << ", ";
        outFile << "y0 = " << box.mY0 << ", ";
        outFile << "z0 = " << box.mZ0 << ", ";
        outFile << "dx = " << box.mDX << ", ";
        outFile << "dy = " << box.mDY << ", ";
        outFile << "dz = " << box.mDZ << std::endl;
    }
}
//----------------------------------------------------------------------------
void Climb3D::Merge (int depth)
{
    int x, y, z, offset, stride;

    for (y = 0; y < mSize; ++y)
    {
        for (z = 0; z < mSize; ++z)
        {
            offset = mSize*(y + mSize*z);
            stride = 1;
            mXMerge[y][z]->SetLevel(mLevel, mData, offset, stride);
        }
    }

    for (x = 0; x < mSize; ++x)
    {
        for (z = 0; z < mSize; ++z)
        {
            offset = x + mSizeSqr*z;
            stride = mSize;
            mYMerge[x][z]->SetLevel(mLevel, mData, offset, stride);
        }
    }

    for (x = 0; x < mSize; ++x)
    {
        for (y = 0; y < mSize; ++y)
        {
            offset = x + mSize*y;
            stride = mSizeSqr;
            mZMerge[x][y]->SetLevel(mLevel, mData, offset, stride);
        }
    }

    Merge(0, 0, 0, 0, 0, 0, 0, mTwoPowerN, depth);
}
//----------------------------------------------------------------------------
bool Climb3D::Merge (int v, int LX, int LY, int LZ, int x0, int y0,
    int z0, int stride, int depth)
{
    if (stride > 1)  // internal nodes
    {
        int hStride = stride/2;
        int vBase = 8*v;
        int v000 = vBase + 1;
        int v100 = vBase + 2;
        int v010 = vBase + 3;
        int v110 = vBase + 4;
        int v001 = vBase + 5;
        int v101 = vBase + 6;
        int v011 = vBase + 7;
        int v111 = vBase + 8;
        int LX0 = 2*LX + 1, LX1 = LX0 + 1;
        int LY0 = 2*LY + 1, LY1 = LY0 + 1;
        int LZ0 = 2*LZ + 1, LZ1 = LZ0 + 1;
        int x1 = x0 + hStride, y1 = y0 + hStride, z1 = z0 + hStride;

        int dm1 = depth - 1;
        bool m000 = Merge(v000, LX0, LY0, LZ0, x0, y0, z0, hStride, dm1);
        bool m100 = Merge(v100, LX1, LY0, LZ0, x1, y0, z0, hStride, dm1);
        bool m010 = Merge(v010, LX0, LY1, LZ0, x0, y1, z0, hStride, dm1);
        bool m110 = Merge(v110, LX1, LY1, LZ0, x1, y1, z0, hStride, dm1);
        bool m001 = Merge(v001, LX0, LY0, LZ1, x0, y0, z1, hStride, dm1);
        bool m101 = Merge(v101, LX1, LY0, LZ1, x1, y0, z1, hStride, dm1);
        bool m011 = Merge(v011, LX0, LY1, LZ1, x0, y1, z1, hStride, dm1);
        bool m111 = Merge(v111, LX1, LY1, LZ1, x1, y1, z1, hStride, dm1);

        MergeBox r000(hStride), r100(hStride), r010(hStride), r110(hStride);
        MergeBox r001(hStride), r101(hStride), r011(hStride), r111(hStride);

        if (depth <= 0)
        {
            if (m000 && m001) DoZMerge(r000, r001, x0, y0, LZ);
            if (m100 && m101) DoZMerge(r100, r101, x1, y0, LZ);
            if (m010 && m011) DoZMerge(r010, r011, x0, y1, LZ);
            if (m110 && m111) DoZMerge(r110, r111, x1, y1, LZ);
            if (m000 && m010) DoYMerge(r000, r010, x0, LY, z0);
            if (m100 && m110) DoYMerge(r100, r110, x1, LY, z0);
            if (m001 && m011) DoYMerge(r001, r011, x0, LY, z1);
            if (m101 && m111) DoYMerge(r101, r111, x1, LY, z1);
            if (m000 && m100) DoXMerge(r000, r100, LX, y0, z0);
            if (m010 && m110) DoXMerge(r010, r110, LX, y1, z0);
            if (m001 && m101) DoXMerge(r001, r101, LX, y0, z1);
            if (m011 && m111) DoXMerge(r011, r111, LX, y1, z1);
        }

        if (depth <= 1)
        {
            if (r000.mValid)
            {
                if (r000.mXStride == stride)
                {
                    if (r000.mYStride == stride)
                    {
                        if (r000.mZStride == stride)
                        {
                            return true;
                        }
                        else
                        {
                            AddBox(x0, y0, z0, stride, stride, hStride, LX,
                                LY, LZ0);
                        }
                    }
                    else
                    {
                        if (r000.mZStride == stride)
                        {
                            AddBox(x0, y0, z0, stride, hStride, stride, LX,
                                LY0, LZ);
                        }
                        else
                        {
                            AddBox(x0, y0, z0, stride, hStride, hStride, LX,
                                LY0, LZ0);
                        }
                    }
                }
                else
                {
                    if (r000.mYStride == stride)
                    {
                        if (r000.mZStride == stride)
                        {
                            AddBox(x0, y0, z0, hStride, stride, stride, LX0,
                                LY, LZ);
                        }
                        else
                        {
                            AddBox(x0, y0, z0, hStride, stride, hStride, LX0,
                                LY, LZ0);
                        }
                    }
                    else
                    {
                        if (r000.mZStride == stride)
                        {
                            AddBox(x0, y0, z0, hStride, hStride, stride, LX0,
                                LY0, LZ);
                        }
                        else if (m000)
                        {
                            AddBox(x0, y0, z0, hStride, hStride, hStride, LX0,
                                LY0, LZ0);
                        }
                    }
                }
            }

            if (r100.mValid)
            {
                if (r100.mYStride == stride)
                {
                    if (r100.mZStride == stride)
                    {
                        AddBox(x1, y0, z0, hStride, stride, stride, LX1, LY,
                            LZ);
                    }
                    else
                    {
                        AddBox(x1, y0, z0, hStride, stride, hStride, LX1, LY,
                            LZ0);
                    }
                }
                else
                {
                    if (r100.mZStride == stride)
                    {
                        AddBox(x1, y0, z0, hStride, hStride, stride, LX1, LY0,
                            LZ);
                    }
                    else if (m100)
                    {
                        AddBox(x1, y0, z0, hStride, hStride, hStride, LX1,
                            LY0, LZ0);
                    }
                }
            }

            if (r010.mValid)
            {
                if (r010.mXStride == stride)
                {
                    if (r010.mZStride == stride)
                    {
                        AddBox(x0, y1, z0, stride, hStride, stride, LX, LY1,
                            LZ);
                    }
                    else
                    {
                        AddBox(x0, y1, z0, stride, hStride, hStride, LX, LY1,
                            LZ0);
                    }
                }
                else
                {
                    if (r010.mZStride == stride)
                    {
                        AddBox(x0, y1, z0, hStride, hStride, stride, LX0, LY1,
                            LZ);
                    }
                    else if (m010)
                    {
                        AddBox(x0, y1, z0, hStride, hStride, hStride, LX0,
                            LY1, LZ0);
                    }
                }
            }

            if (r001.mValid)
            {
                if (r001.mXStride == stride)
                {
                    if (r001.mYStride == stride)
                    {
                        AddBox(x0, y0, z1, stride, stride, hStride, LX, LY,
                            LZ1);
                    }
                    else
                    {
                        AddBox(x0, y0, z1, stride, hStride, hStride, LX, LY0,
                            LZ1);
                    }
                }
                else
                {
                    if (r001.mYStride == stride)
                    {
                        AddBox(x0, y0, z1, hStride, stride, hStride, LX0, LY,
                            LZ1);
                    }
                    else if (m001)
                    {
                        AddBox(x0, y0, z1, hStride, hStride, hStride, LX0,
                            LY0, LZ1);
                    }
                }
            }

            if (r110.mValid)
            {
                if (r110.mZStride == stride)
                {
                    AddBox(x1, y1, z0, hStride, hStride, stride, LX1, LY1,
                        LZ);
                }
                else if (m110)
                {
                    AddBox(x1, y1, z0, hStride, hStride, hStride, LX1, LY1,
                        LZ0);
                }
            }

            if (r101.mValid)
            {
                if (r101.mYStride == stride)
                {
                    AddBox(x1, y0, z1, hStride, stride, hStride, LX1, LY,
                        LZ1);
                }
                else if (m101)
                {
                    AddBox(x1, y0, z1, hStride, hStride, hStride, LX1, LY0,
                        LZ1);
                }
            }

            if (r011.mValid)
            {
                if (r011.mXStride == stride)
                {
                    AddBox(x0, y1, z1, stride, hStride, hStride, LX, LY1,
                        LZ1);
                }
                else if (m011)
                {
                    AddBox(x0, y1, z1, hStride, hStride, hStride, LX0, LY1,
                        LZ1);
                }
            }

            if (r111.mValid && m111)
            {
                AddBox(x1, y1, z1, hStride, hStride, hStride, LX1, LY1, LZ1);
            }
        }
        return false;
    }
    else  // leaf nodes
    {
        if (mFixBoundary)
        {
            // Do not allow boundary voxels to merge with any other voxels.
            AddBox(x0, y0, z0, 1, 1, 1, LX, LY, LZ);
            return false;
        }

        // A leaf box is mergeable with neighbors as long as all its faces
        // have 0 or 2 sign changes on the edges.  That is,  a face may not
        // have sign changes on all four edges.  If it does,  the resulting
        // box for tessellating is 1x1x1 and is handled separately from boxes
        // of larger dimensions.

        // xmin face
        int z1 = z0+1;
        unsigned char rt0 = mYMerge[x0][z0]->GetRootType(LY);
        unsigned char rt1 = mYMerge[x0][z1]->GetRootType(LY);
        if ((rt0 | rt1) == LinearMergeTree::CFG_MULT)
        {
            AddBox(x0, y0, z0, 1, 1, 1, LX, LY, LZ);
            return false;
        }

        // xmax face
        int x1 = x0+1;
        rt0 = mYMerge[x1][z0]->GetRootType(LY);
        rt1 = mYMerge[x1][z1]->GetRootType(LY);
        if ((rt0 | rt1) == LinearMergeTree::CFG_MULT)
        {
            AddBox(x0, y0, z0, 1, 1, 1, LX, LY, LZ);
            return false;
        }

        // ymin face
        rt0 = mZMerge[x0][y0]->GetRootType(LZ);
        rt1 = mZMerge[x1][y0]->GetRootType(LZ);
        if ((rt0 | rt1) == LinearMergeTree::CFG_MULT)
        {
            AddBox(x0, y0, z0, 1, 1, 1, LX, LY, LZ);
            return false;
        }

        // ymax face
        int y1 = y0+1;
        rt0 = mZMerge[x0][y1]->GetRootType(LZ);
        rt1 = mZMerge[x1][y1]->GetRootType(LZ);
        if ((rt0 | rt1) == LinearMergeTree::CFG_MULT)
        {
            AddBox(x0, y0, z0, 1, 1, 1, LX, LY, LZ);
            return false;
        }

        // zmin face
        rt0 = mXMerge[y0][z0]->GetRootType(LX);
        rt1 = mXMerge[y1][z0]->GetRootType(LX);
        if ((rt0 | rt1) == LinearMergeTree::CFG_MULT)
        {
            AddBox(x0, y0, z0, 1, 1, 1, LX, LY, LZ);
            return false;
        }

        // zmax face
        rt0 = mXMerge[y0][z1]->GetRootType(LX);
        rt1 = mXMerge[y1][z1]->GetRootType(LX);
        if ((rt0 | rt1) == LinearMergeTree::CFG_MULT)
        {
            AddBox(x0, y0, z0, 1, 1, 1, LX, LY, LZ);
            return false;
        }

        return true;
    }
}
//----------------------------------------------------------------------------
bool Climb3D::DoXMerge (MergeBox& r0, MergeBox& r1, int LX, int y0, int z0)
{
    if (!r0.mValid || !r1.mValid
    ||  r0.mYStride != r1.mYStride || r0.mZStride != r1.mZStride)
    {
        return false;
    }

    // Boxes are potentially x-mergeable.
    int y1 = y0 + r0.mYStride, z1 = z0 + r0.mZStride;
    int incr = 0, decr = 0;
    for (int y = y0; y <= y1; ++y)
    {
        for (int z = z0; z <= z1; ++z)
        {
            switch (mXMerge[y][z]->GetRootType(LX))
            {
            case LinearMergeTree::CFG_MULT:
                return false;
            case LinearMergeTree::CFG_INCR:
                ++incr;
                break;
            case LinearMergeTree::CFG_DECR:
                ++decr;
                break;
            }
        }
    }

    if (incr != 0 && decr != 0)
    {
        return false;
    }

    // Strongly mono, x-merge the boxes.
    r0.mXStride *= 2;
    r1.mValid = false;
    return true;
}
//----------------------------------------------------------------------------
bool Climb3D::DoYMerge (MergeBox& r0, MergeBox& r1, int x0, int LY, int z0)
{
    if ( !r0.mValid || !r1.mValid
    ||   r0.mXStride != r1.mXStride || r0.mZStride != r1.mZStride)
    {
        return false;
    }

    // Boxes are potentially y-mergeable.
    int x1 = x0 + r0.mXStride, z1 = z0 + r0.mZStride;
    int incr = 0, decr = 0;
    for (int x = x0; x <= x1; ++x)
    {
        for (int z = z0; z <= z1; ++z)
        {
            switch (mYMerge[x][z]->GetRootType(LY))
            {
            case LinearMergeTree::CFG_MULT:
                return false;
            case LinearMergeTree::CFG_INCR:
                ++incr;
                break;
            case LinearMergeTree::CFG_DECR:
                ++decr;
                break;
            }
        }
    }

    if (incr != 0 && decr != 0)
    {
        return false;
    }

    // Strongly mono, y-merge the boxes.
    r0.mYStride *= 2;
    r1.mValid = false;
    return true;
}
//----------------------------------------------------------------------------
bool Climb3D::DoZMerge (MergeBox& r0, MergeBox& r1, int x0, int y0, int LZ)
{
    if (!r0.mValid || !r1.mValid
    ||  r0.mXStride != r1.mXStride
    ||  r0.mYStride != r1.mYStride)
    {
        return false;
    }

    // Boxes are potentially z-mergeable.
    int x1 = x0 + r0.mXStride, y1 = y0 + r0.mYStride;
    int incr = 0, decr = 0;
    for (int x = x0; x <= x1; ++x)
    {
        for (int y = y0; y <= y1; ++y)
        {
            switch (mZMerge[x][y]->GetRootType(LZ))
            {
            case LinearMergeTree::CFG_MULT:
                return false;
            case LinearMergeTree::CFG_INCR:
                ++incr;
                break;
            case LinearMergeTree::CFG_DECR:
                ++decr;
                break;
            }
        }
    }

    if (incr != 0 && decr != 0)
    {
        return false;
    }

    // Strongly mono, z-merge the boxes.
    r0.mZStride *= 2;
    r1.mValid = false;
    return true;
}
//----------------------------------------------------------------------------
void Climb3D::AddBox (int x0, int y0, int z0, int dx, int dy, int dz, int LX,
    int LY, int LZ)
{
    OctBox box(x0, y0, z0, dx, dy, dz, LX, LY, LZ);
    mBoxes.push_back(box);

    // Mark box edges in linear merge trees.  This information will be used
    // in the 'Split' call.
    mXMerge[box.mY0][box.mZ0]->SetEdge(box.mLX);
    mXMerge[box.mY0][box.mZ1]->SetEdge(box.mLX);
    mXMerge[box.mY1][box.mZ0]->SetEdge(box.mLX);
    mXMerge[box.mY1][box.mZ1]->SetEdge(box.mLX);
    mYMerge[box.mX0][box.mZ0]->SetEdge(box.mLY);
    mYMerge[box.mX0][box.mZ1]->SetEdge(box.mLY);
    mYMerge[box.mX1][box.mZ0]->SetEdge(box.mLY);
    mYMerge[box.mX1][box.mZ1]->SetEdge(box.mLY);
    mZMerge[box.mX0][box.mY0]->SetEdge(box.mLZ);
    mZMerge[box.mX0][box.mY1]->SetEdge(box.mLZ);
    mZMerge[box.mX1][box.mY0]->SetEdge(box.mLZ);
    mZMerge[box.mX1][box.mY1]->SetEdge(box.mLZ);
}
//----------------------------------------------------------------------------
void Climb3D::Tessellate (std::vector<Vector3f>& positions,
    std::vector<TriangleKey>& triangles)
{
    const int numBoxes = (int)mBoxes.size();
    for (int i = 0; i < numBoxes; ++i)
    {
        const OctBox& box = mBoxes[i];

        // Get vertices on edges of box.
        VETable table;
        unsigned int type;
        GetVertices(box, type, table);
        if (type == 0)
        {
            continue;
        }

        // Add wireframe edges to table, add face-vertices if necessary.
        if (box.mDX > 1 || box.mDY > 1 || box.mDZ > 1)
        {
            // Box is larger than voxel, each face has at most one edge.
            GetXMinEdgesM(box, type, table);
            GetXMaxEdgesM(box, type, table);
            GetYMinEdgesM(box, type, table);
            GetYMaxEdgesM(box, type, table);
            GetZMinEdgesM(box, type, table);
            GetZMaxEdgesM(box, type, table);

            if (table.GetNumVertices() > 18)
            {
                table.RemoveTrianglesSE(positions, triangles);
            }
            else
            {
                table.RemoveTrianglesEC(positions, triangles);
            }
        }
        else
        {
            // 1x1x1 voxel, do full edge analysis, no splitting required
            GetXMinEdgesS(box, type, table);
            GetXMaxEdgesS(box, type, table);
            GetYMinEdgesS(box, type, table);
            GetYMaxEdgesS(box, type, table);
            GetZMinEdgesS(box, type, table);
            GetZMaxEdgesS(box, type, table);
            table.RemoveTrianglesEC(positions, triangles);
        }
    }
}
//----------------------------------------------------------------------------
float Climb3D::GetXInterp (int x, int y, int z) const
{
    int index = x + mSize*(y + mSize*z);
    float f0 = mData[index];
    index++;
    float f1 = mData[index];
    return (float)x + (mLevel - f0)/(f1 - f0);
}
//----------------------------------------------------------------------------
float Climb3D::GetYInterp (int x, int y, int z) const
{
    int index = x + mSize*(y + mSize*z);
    float f0 = mData[index];
    index += mSize;
    float f1 = mData[index];
    return (float)y + (mLevel - f0)/(f1 - f0);
}
//----------------------------------------------------------------------------
float Climb3D::GetZInterp (int x, int y, int z) const
{
    int index = x + mSize*(y + mSize*z);
    float f0 = mData[index];
    index += mSizeSqr;
    float f1 = mData[index];
    return (float)z + (mLevel - f0)/(f1 - f0);
}
//----------------------------------------------------------------------------
void Climb3D::GetVertices (const OctBox& box, unsigned int& type,
    VETable& table)
{
    int root;
    type = 0;

    // xmin-ymin edge
    root = mZMerge[box.mX0][box.mY0]->GetZeroBase(box.mLZ);
    if (root != -1)
    {
        type |= EB_XMIN_YMIN;
        table.Insert(EI_XMIN_YMIN,
            (float)box.mX0,
            (float)box.mY0,
            GetZInterp(box.mX0, box.mY0, root));
    }

    // xmin-ymax edge
    root = mZMerge[box.mX0][box.mY1]->GetZeroBase(box.mLZ);
    if (root != -1)
    {
        type |= EB_XMIN_YMAX;
        table.Insert(EI_XMIN_YMAX,
            (float)box.mX0,
            (float)box.mY1,
            GetZInterp(box.mX0, box.mY1, root));
    }

    // xmax-ymin edge
    root = mZMerge[box.mX1][box.mY0]->GetZeroBase(box.mLZ);
    if (root != -1)
    {
        type |= EB_XMAX_YMIN;
        table.Insert(EI_XMAX_YMIN,
            (float)box.mX1,
            (float)box.mY0,
            GetZInterp(box.mX1, box.mY0, root));
    }

    // xmax-ymax edge
    root = mZMerge[box.mX1][box.mY1]->GetZeroBase(box.mLZ);
    if (root != -1)
    {
        type |= EB_XMAX_YMAX;
        table.Insert(EI_XMAX_YMAX,
            (float)box.mX1,
            (float)box.mY1,
            GetZInterp(box.mX1, box.mY1, root));
    }

    // xmin-zmin edge
    root = mYMerge[box.mX0][box.mZ0]->GetZeroBase(box.mLY);
    if (root != -1)
    {
        type |= EB_XMIN_ZMIN;
        table.Insert(EI_XMIN_ZMIN,
            (float)box.mX0,
            GetYInterp(box.mX0, root, box.mZ0),
            (float)box.mZ0);
    }

    // xmin-zmax edge
    root = mYMerge[box.mX0][box.mZ1]->GetZeroBase(box.mLY);
    if (root != -1)
    {
        type |= EB_XMIN_ZMAX;
        table.Insert(EI_XMIN_ZMAX,
            (float)box.mX0,
            GetYInterp(box.mX0, root, box.mZ1),
            (float)box.mZ1);
    }

    // xmax-zmin edge
    root = mYMerge[box.mX1][box.mZ0]->GetZeroBase(box.mLY);
    if (root != -1)
    {
        type |= EB_XMAX_ZMIN;
        table.Insert(EI_XMAX_ZMIN,
            (float)box.mX1,
            GetYInterp(box.mX1, root, box.mZ0),
            (float)box.mZ0);
    }

    // xmax-zmax edge
    root = mYMerge[box.mX1][box.mZ1]->GetZeroBase(box.mLY);
    if (root != -1)
    {
        type |= EB_XMAX_ZMAX;
        table.Insert(EI_XMAX_ZMAX,
            (float)box.mX1,
            GetYInterp(box.mX1, root, box.mZ1),
            (float)box.mZ1);
    }

    // ymin-zmin edge
    root = mXMerge[box.mY0][box.mZ0]->GetZeroBase(box.mLX);
    if (root != -1)
    {
        type |= EB_YMIN_ZMIN;
        table.Insert(EI_YMIN_ZMIN,
            GetXInterp(root, box.mY0, box.mZ0),
            (float)box.mY0,
            (float)box.mZ0);
    }

    // ymin-zmax edge
    root = mXMerge[box.mY0][box.mZ1]->GetZeroBase(box.mLX);
    if (root != -1)
    {
        type |= EB_YMIN_ZMAX;
        table.Insert(EI_YMIN_ZMAX,
            GetXInterp(root, box.mY0, box.mZ1),
            (float)box.mY0,
            (float)box.mZ1);
    }

    // ymax-zmin edge
    root = mXMerge[box.mY1][box.mZ0]->GetZeroBase(box.mLX);
    if (root != -1)
    {
        type |= EB_YMAX_ZMIN;
        table.Insert(EI_YMAX_ZMIN,
            GetXInterp(root, box.mY1, box.mZ0),
            (float)box.mY1,
            (float)box.mZ0);
    }

    // ymax-zmax edge
    root = mXMerge[box.mY1][box.mZ1]->GetZeroBase(box.mLX);
    if (root != -1)
    {
        type |= EB_YMAX_ZMAX;
        table.Insert(EI_YMAX_ZMAX,
            GetXInterp(root, box.mY1, box.mZ1),
            (float)box.mY1,
            (float)box.mZ1);
    }
}
//----------------------------------------------------------------------------
void Climb3D::GetXMinEdgesS (const OctBox& box, unsigned int type,
    VETable& table)
{
    unsigned int faceType = 0;
    if (type & EB_XMIN_YMIN) { faceType |= 0x01; }
    if (type & EB_XMIN_YMAX) { faceType |= 0x02; }
    if (type & EB_XMIN_ZMIN) { faceType |= 0x04; }
    if (type & EB_XMIN_ZMAX) { faceType |= 0x08; }

    switch (faceType)
    {
    case  0: return;
    case  3: table.Insert(Edge3(EI_XMIN_YMIN, EI_XMIN_YMAX)); break;
    case  5: table.Insert(Edge3(EI_XMIN_YMIN, EI_XMIN_ZMIN)); break;
    case  6: table.Insert(Edge3(EI_XMIN_YMAX, EI_XMIN_ZMIN)); break;
    case  9: table.Insert(Edge3(EI_XMIN_YMIN, EI_XMIN_ZMAX)); break;
    case 10: table.Insert(Edge3(EI_XMIN_YMAX, EI_XMIN_ZMAX)); break;
    case 12: table.Insert(Edge3(EI_XMIN_ZMIN, EI_XMIN_ZMAX)); break;
    case 15:
    {
        // Four vertices, one per edge, need to disambiguate.
        int i = box.mX0 + mSize*(box.mY0 + mSize*box.mZ0);
        float f00 = mData[i];  // F(x,y,z)
        i += mSize;
        float f10 = mData[i];  // F(x,y+1,z)
        i += mSizeSqr;
        float f11 = mData[i];  // F(x,y+1,z+1)
        i -= mSize;
        float f01 = mData[i];  // F(x,y,z+1)
        float det = f00*f11 - f01*f10;

        if (det > 0.0f)
        {
            // Disjoint hyperbolic segments, pair <P0,P2>, <P1,P3>.
            table.Insert(Edge3(EI_XMIN_YMIN, EI_XMIN_ZMIN));
            table.Insert(Edge3(EI_XMIN_YMAX, EI_XMIN_ZMAX));
        }
        else if (det < 0.0f)
        {
            // Disjoint hyperbolic segments, pair <P0,P3>, <P1,P2>.
            table.Insert(Edge3(EI_XMIN_YMIN, EI_XMIN_ZMAX));
            table.Insert(Edge3(EI_XMIN_YMAX, EI_XMIN_ZMIN));
        }
        else
        {
            // Plus-sign configuration, add branch point to tessellation.
            table.Insert(FI_XMIN,
                table.GetPosition(EI_XMIN_ZMIN)[0],
                table.GetPosition(EI_XMIN_ZMIN)[1],
                table.GetPosition(EI_XMIN_YMIN)[2]);

            // Add edges sharing the branch point.
            table.Insert(Edge3(EI_XMIN_YMIN, FI_XMIN));
            table.Insert(Edge3(EI_XMIN_YMAX, FI_XMIN));
            table.Insert(Edge3(EI_XMIN_ZMIN, FI_XMIN));
            table.Insert(Edge3(EI_XMIN_ZMAX, FI_XMIN));
        }
        break;
    }
    default:
        assertion(false, "Unexpected case.\n");
    }
}
//----------------------------------------------------------------------------
void Climb3D::GetXMaxEdgesS (const OctBox& box, unsigned int type,
    VETable& table)
{
    unsigned int faceType = 0;
    if (type & EB_XMAX_YMIN) { faceType |= 0x01; }
    if (type & EB_XMAX_YMAX) { faceType |= 0x02; }
    if (type & EB_XMAX_ZMIN) { faceType |= 0x04; }
    if (type & EB_XMAX_ZMAX) { faceType |= 0x08; }

    switch (faceType)
    {
    case  0: return;
    case  3: table.Insert(Edge3(EI_XMAX_YMIN, EI_XMAX_YMAX)); break;
    case  5: table.Insert(Edge3(EI_XMAX_YMIN, EI_XMAX_ZMIN)); break;
    case  6: table.Insert(Edge3(EI_XMAX_YMAX, EI_XMAX_ZMIN)); break;
    case  9: table.Insert(Edge3(EI_XMAX_YMIN, EI_XMAX_ZMAX)); break;
    case 10: table.Insert(Edge3(EI_XMAX_YMAX, EI_XMAX_ZMAX)); break;
    case 12: table.Insert(Edge3(EI_XMAX_ZMIN, EI_XMAX_ZMAX)); break;
    case 15:
    {
        // Four vertices, one per edge, need to disambiguate.
        int i = box.mX1 + mSize*(box.mY0 + mSize*box.mZ0);
        float f00 = mData[i];  // F(x,y,z)
        i += mSize;
        float f10 = mData[i];  // F(x,y+1,z)
        i += mSizeSqr;
        float f11 = mData[i];  // F(x,y+1,z+1)
        i -= mSize;
        float f01 = mData[i];  // F(x,y,z+1)
        float det = f00*f11 - f01*f10;

        if (det > 0.0f)
        {
            // Disjoint hyperbolic segments, pair <P0,P2>, <P1,P3>.
            table.Insert(Edge3(EI_XMAX_YMIN, EI_XMAX_ZMIN));
            table.Insert(Edge3(EI_XMAX_YMAX, EI_XMAX_ZMAX));
        }
        else if (det < 0.0f)
        {
            // Disjoint hyperbolic segments, pair <P0,P3>, <P1,P2>.
            table.Insert(Edge3(EI_XMAX_YMIN, EI_XMAX_ZMAX));
            table.Insert(Edge3(EI_XMAX_YMAX, EI_XMAX_ZMIN));
        }
        else
        {
            // Plus-sign configuration, add branch point to tessellation.
            table.Insert(FI_XMAX,
                table.GetPosition(EI_XMAX_ZMIN)[0],
                table.GetPosition(EI_XMAX_ZMIN)[1],
                table.GetPosition(EI_XMAX_YMIN)[2]);

            // Add edges sharing the branch point.
            table.Insert(Edge3(EI_XMAX_YMIN, FI_XMAX));
            table.Insert(Edge3(EI_XMAX_YMAX, FI_XMAX));
            table.Insert(Edge3(EI_XMAX_ZMIN, FI_XMAX));
            table.Insert(Edge3(EI_XMAX_ZMAX, FI_XMAX));
        }
        break;
    }
    default:
        assertion(false, "Unexpected case.\n");
    }
}
//----------------------------------------------------------------------------
void Climb3D::GetYMinEdgesS (const OctBox& box, unsigned int type,
    VETable& table)
{
    unsigned int faceType = 0;
    if (type & EB_XMIN_YMIN) { faceType |= 0x01; }
    if (type & EB_XMAX_YMIN) { faceType |= 0x02; }
    if (type & EB_YMIN_ZMIN) { faceType |= 0x04; }
    if (type & EB_YMIN_ZMAX) { faceType |= 0x08; }

    switch (faceType)
    {
    case  0: return;
    case  3: table.Insert(Edge3(EI_XMIN_YMIN, EI_XMAX_YMIN)); break;
    case  5: table.Insert(Edge3(EI_XMIN_YMIN, EI_YMIN_ZMIN)); break;
    case  6: table.Insert(Edge3(EI_XMAX_YMIN, EI_YMIN_ZMIN)); break;
    case  9: table.Insert(Edge3(EI_XMIN_YMIN, EI_YMIN_ZMAX)); break;
    case 10: table.Insert(Edge3(EI_XMAX_YMIN, EI_YMIN_ZMAX)); break;
    case 12: table.Insert(Edge3(EI_YMIN_ZMIN, EI_YMIN_ZMAX)); break;
    case 15:
    {
        // Four vertices, one per edge, need to disambiguate.
        int i = box.mX0 + mSize*(box.mY0 + mSize*box.mZ0);
        float f00 = mData[i];  // F(x,y,z)
        i++;
        float f10 = mData[i];  // F(x+1,y,z)
        i += mSizeSqr;
        float f11 = mData[i];  // F(x+1,y,z+1)
        i--;
        float f01 = mData[i];  // F(x,y,z+1)
        float det = f00*f11 - f01*f10;

        if (det > 0.0f)
        {
            // Disjoint hyperbolic segments, pair <P0,P2>, <P1,P3>.
            table.Insert(Edge3(EI_XMIN_YMIN, EI_YMIN_ZMIN));
            table.Insert(Edge3(EI_XMAX_YMIN, EI_YMIN_ZMAX));
        }
        else if (det < 0.0f)
        {
            // Disjoint hyperbolic segments, pair <P0,P3>, <P1,P2>.
            table.Insert(Edge3(EI_XMIN_YMIN, EI_YMIN_ZMAX));
            table.Insert(Edge3(EI_XMAX_YMIN, EI_YMIN_ZMIN));
        }
        else
        {
            // Plus-sign configuration, add branch point to tessellation.
            table.Insert(FI_YMIN,
                table.GetPosition(EI_YMIN_ZMIN)[0],
                table.GetPosition(EI_XMIN_YMIN)[1],
                table.GetPosition(EI_XMIN_YMIN)[2]);

            // Add edges sharing the branch point.
            table.Insert(Edge3(EI_XMIN_YMIN, FI_YMIN));
            table.Insert(Edge3(EI_XMAX_YMIN, FI_YMIN));
            table.Insert(Edge3(EI_YMIN_ZMIN, FI_YMIN));
            table.Insert(Edge3(EI_YMIN_ZMAX, FI_YMIN));
        }
        break;
    }
    default:
        assertion(false, "Unexpected case.\n");
    }
}
//----------------------------------------------------------------------------
void Climb3D::GetYMaxEdgesS (const OctBox& box, unsigned int type,
    VETable& table)
{
    unsigned int faceType = 0;
    if (type & EB_XMIN_YMAX) { faceType |= 0x01; }
    if (type & EB_XMAX_YMAX) { faceType |= 0x02; }
    if (type & EB_YMAX_ZMIN) { faceType |= 0x04; }
    if (type & EB_YMAX_ZMAX) { faceType |= 0x08; }

    switch (faceType)
    {
    case  0: return;
    case  3: table.Insert(Edge3(EI_XMIN_YMAX, EI_XMAX_YMAX)); break;
    case  5: table.Insert(Edge3(EI_XMIN_YMAX, EI_YMAX_ZMIN)); break;
    case  6: table.Insert(Edge3(EI_XMAX_YMAX, EI_YMAX_ZMIN)); break;
    case  9: table.Insert(Edge3(EI_XMIN_YMAX, EI_YMAX_ZMAX)); break;
    case 10: table.Insert(Edge3(EI_XMAX_YMAX, EI_YMAX_ZMAX)); break;
    case 12: table.Insert(Edge3(EI_YMAX_ZMIN, EI_YMAX_ZMAX)); break;
    case 15:
    {
        // Four vertices, one per edge, need to disambiguate.
        int i = box.mX0 + mSize*(box.mY1 + mSize*box.mZ0);
        float f00 = mData[i];  // F(x,y,z)
        i++;
        float f10 = mData[i];  // F(x+1,y,z)
        i += mSizeSqr;
        float f11 = mData[i];  // F(x+1,y,z+1)
        i--;
        float f01 = mData[i];  // F(x,y,z+1)
        float det = f00*f11 - f01*f10;

        if (det > 0.0f)
        {
            // Disjoint hyperbolic segments, pair <P0,P2>, <P1,P3>.
            table.Insert(Edge3(EI_XMIN_YMAX, EI_YMAX_ZMIN));
            table.Insert(Edge3(EI_XMAX_YMAX, EI_YMAX_ZMAX));
        }
        else if (det < 0.0f)
        {
            // Disjoint hyperbolic segments, pair <P0,P3>, <P1,P2>.
            table.Insert(Edge3(EI_XMIN_YMAX, EI_YMAX_ZMAX));
            table.Insert(Edge3(EI_XMAX_YMAX, EI_YMAX_ZMIN));
        }
        else
        {
            // Plus-sign configuration, add branch point to tessellation.
            table.Insert(FI_YMAX,
                table.GetPosition(EI_YMAX_ZMIN)[0],
                table.GetPosition(EI_XMIN_YMAX)[1],
                table.GetPosition(EI_XMIN_YMAX)[2]);

            // Add edges sharing the branch point.
            table.Insert(Edge3(EI_XMIN_YMAX, FI_YMAX));
            table.Insert(Edge3(EI_XMAX_YMAX, FI_YMAX));
            table.Insert(Edge3(EI_YMAX_ZMIN, FI_YMAX));
            table.Insert(Edge3(EI_YMAX_ZMAX, FI_YMAX));
        }
        break;
    }
    default:
        assertion(false, "Unexpected case.\n");
    }
}
//----------------------------------------------------------------------------
void Climb3D::GetZMinEdgesS (const OctBox& box, unsigned int type,
    VETable& table)
{
    unsigned int faceType = 0;
    if (type & EB_XMIN_ZMIN) { faceType |= 0x01; }
    if (type & EB_XMAX_ZMIN) { faceType |= 0x02; }
    if (type & EB_YMIN_ZMIN) { faceType |= 0x04; }
    if (type & EB_YMAX_ZMIN) { faceType |= 0x08; }

    switch (faceType)
    {
    case  0: return;
    case  3: table.Insert(Edge3(EI_XMIN_ZMIN, EI_XMAX_ZMIN)); break;
    case  5: table.Insert(Edge3(EI_XMIN_ZMIN, EI_YMIN_ZMIN)); break;
    case  6: table.Insert(Edge3(EI_XMAX_ZMIN, EI_YMIN_ZMIN)); break;
    case  9: table.Insert(Edge3(EI_XMIN_ZMIN, EI_YMAX_ZMIN)); break;
    case 10: table.Insert(Edge3(EI_XMAX_ZMIN, EI_YMAX_ZMIN)); break;
    case 12: table.Insert(Edge3(EI_YMIN_ZMIN, EI_YMAX_ZMIN)); break;
    case 15:
    {
        // Four vertices, one per edge, need to disambiguate.
        int i = box.mX0 + mSize*(box.mY0 + mSize*box.mZ0);
        float f00 = mData[i];  // F(x,y,z)
        i++;
        float f10 = mData[i];  // F(x+1,y,z)
        i += mSize;
        float f11 = mData[i];  // F(x+1,y+1,z)
        i--;
        float f01 = mData[i];  // F(x,y+1,z)
        float det = f00*f11 - f01*f10;

        if (det > 0.0f)
        {
            // Disjoint hyperbolic segments, pair <P0,P2>, <P1,P3>.
            table.Insert(Edge3(EI_XMIN_ZMIN, EI_YMIN_ZMIN));
            table.Insert(Edge3(EI_XMAX_ZMIN, EI_YMAX_ZMIN));
        }
        else if (det < 0.0f)
        {
            // Disjoint hyperbolic segments, pair <P0,P3>, <P1,P2>.
            table.Insert(Edge3(EI_XMIN_ZMIN, EI_YMAX_ZMIN));
            table.Insert(Edge3(EI_XMAX_ZMIN, EI_YMIN_ZMIN));
        }
        else
        {
            // Plus-sign configuration, add branch point to tessellation.
            table.Insert(FI_ZMIN,
                table.GetPosition(EI_YMIN_ZMIN)[0],
                table.GetPosition(EI_XMIN_ZMIN)[1],
                table.GetPosition(EI_XMIN_ZMIN)[2]);

            // Add edges sharing the branch point.
            table.Insert(Edge3(EI_XMIN_ZMIN, FI_ZMIN));
            table.Insert(Edge3(EI_XMAX_ZMIN, FI_ZMIN));
            table.Insert(Edge3(EI_YMIN_ZMIN, FI_ZMIN));
            table.Insert(Edge3(EI_YMAX_ZMIN, FI_ZMIN));
        }
        break;
    }
    default:
        assertion(false, "Unexpected case.\n");
    }
}
//----------------------------------------------------------------------------
void Climb3D::GetZMaxEdgesS (const OctBox& box, unsigned int type,
    VETable& table)
{
    unsigned int faceType = 0;
    if (type & EB_XMIN_ZMAX) { faceType |= 0x01; }
    if (type & EB_XMAX_ZMAX) { faceType |= 0x02; }
    if (type & EB_YMIN_ZMAX) { faceType |= 0x04; }
    if (type & EB_YMAX_ZMAX) { faceType |= 0x08; }

    switch (faceType)
    {
    case  0: return;
    case  3: table.Insert(Edge3(EI_XMIN_ZMAX, EI_XMAX_ZMAX)); break;
    case  5: table.Insert(Edge3(EI_XMIN_ZMAX, EI_YMIN_ZMAX)); break;
    case  6: table.Insert(Edge3(EI_XMAX_ZMAX, EI_YMIN_ZMAX)); break;
    case  9: table.Insert(Edge3(EI_XMIN_ZMAX, EI_YMAX_ZMAX)); break;
    case 10: table.Insert(Edge3(EI_XMAX_ZMAX, EI_YMAX_ZMAX)); break;
    case 12: table.Insert(Edge3(EI_YMIN_ZMAX, EI_YMAX_ZMAX)); break;
    case 15:
    {
        // Four vertices, one per edge, need to disambiguate.
        int i = box.mX0 + mSize*(box.mY0 + mSize*box.mZ1);
        float f00 = mData[i];  // F(x,y,z)
        i++;
        float f10 = mData[i];  // F(x+1,y,z)
        i += mSize;
        float f11 = mData[i];  // F(x+1,y+1,z)
        i--;
        float f01 = mData[i];  // F(x,y+1,z)
        float det = f00*f11 - f01*f10;

        if (det > 0.0f)
        {
            // Disjoint hyperbolic segments, pair <P0,P2>, <P1,P3>.
            table.Insert(Edge3(EI_XMIN_ZMAX, EI_YMIN_ZMAX));
            table.Insert(Edge3(EI_XMAX_ZMAX, EI_YMAX_ZMAX));
        }
        else if (det < 0.0f)
        {
            // Disjoint hyperbolic segments, pair <P0,P3>, <P1,P2>.
            table.Insert(Edge3(EI_XMIN_ZMAX, EI_YMAX_ZMAX));
            table.Insert(Edge3(EI_XMAX_ZMAX, EI_YMIN_ZMAX));
        }
        else
        {
            // Plus-sign configuration, add branch point to tessellation.
            table.Insert(FI_ZMAX,
                table.GetPosition(EI_YMIN_ZMAX)[0],
                table.GetPosition(EI_XMIN_ZMAX)[1],
                table.GetPosition(EI_XMIN_ZMAX)[2]);

            // Add edges sharing the branch point.
            table.Insert(Edge3(EI_XMIN_ZMAX, FI_ZMAX));
            table.Insert(Edge3(EI_XMAX_ZMAX, FI_ZMAX));
            table.Insert(Edge3(EI_YMIN_ZMAX, FI_ZMAX));
            table.Insert(Edge3(EI_YMAX_ZMAX, FI_ZMAX));
        }
        break;
    }
    default:
        assertion(false, "Unexpected case.\n");
    }
}
//----------------------------------------------------------------------------
bool Climb3D::Sort0::operator() (const Vector3f& arg0, const Vector3f& arg1)
    const
{
    if (arg0[0] < arg1[0]) { return true; }
    if (arg0[0] > arg1[0]) { return false; }
    if (arg0[1] < arg1[1]) { return true; }
    if (arg0[1] > arg1[1]) { return false; }
    return arg0[2] < arg1[2];
}
//----------------------------------------------------------------------------
bool Climb3D::Sort1::operator() (const Vector3f& arg0, const Vector3f& arg1)
    const
{
    if (arg0[2] < arg1[2]) { return true; }
    if (arg0[2] > arg1[2]) { return false; }
    if (arg0[0] < arg1[0]) { return true; }
    if (arg0[0] > arg1[0]) { return false; }
    return arg0[1] < arg1[1];
}
//----------------------------------------------------------------------------
bool Climb3D::Sort2::operator() (const Vector3f& arg0, const Vector3f& arg1)
    const
{
    if (arg0[1] < arg1[1]) { return true; }
    if (arg0[1] > arg1[1]) { return false; }
    if (arg0[2] < arg1[2]) { return true; }
    if (arg0[2] > arg1[2]) { return false; }
    return arg0[0] < arg1[0];
}
//----------------------------------------------------------------------------
void Climb3D::GetZMinEdgesM (const OctBox& box, unsigned int type,
    VETable& table)
{
    unsigned int faceType = 0;
    if (type & EB_XMIN_ZMIN) { faceType |= 0x01; }
    if (type & EB_XMAX_ZMIN) { faceType |= 0x02; }
    if (type & EB_YMIN_ZMIN) { faceType |= 0x04; }
    if (type & EB_YMAX_ZMIN) { faceType |= 0x08; }

    int end0 = 0, end1 = 0;
    switch (faceType)
    {
    case  0: return;
    case  3: end0 = EI_XMIN_ZMIN; end1 = EI_XMAX_ZMIN; break;
    case  5: end0 = EI_XMIN_ZMIN; end1 = EI_YMIN_ZMIN; break;
    case  6: end0 = EI_YMIN_ZMIN; end1 = EI_XMAX_ZMIN; break;
    case  9: end0 = EI_XMIN_ZMIN; end1 = EI_YMAX_ZMIN; break;
    case 10: end0 = EI_YMAX_ZMIN; end1 = EI_XMAX_ZMIN; break;
    case 12: end0 = EI_YMIN_ZMIN; end1 = EI_YMAX_ZMIN; break;
    default: assertion(false, "Unexpected case.\n");
    }

    std::set<Vector3f,Sort0> vSet;
    LinearMergeTree* merge;
    int root;

    for (int x = box.mX0 + 1; x < box.mX1; ++x)
    {
        merge = mYMerge[x][box.mZ0];
        if (merge->IsZeroEdge(box.mLY) || merge->HasZeroSubedge(box.mLY))
        {
            root = merge->GetZeroBase(box.mLY);
            assertion(root != -1, "Unexpected condition.\n");
            vSet.insert(Vector3f(
                (float)x, GetYInterp(x, root, box.mZ0), (float)box.mZ0));
        }
    }

    for (int y = box.mY0 + 1; y < box.mY1; ++y)
    {
        merge = mXMerge[y][box.mZ0];
        if (merge->IsZeroEdge(box.mLX) || merge->HasZeroSubedge(box.mLX))
        {
            root = merge->GetZeroBase(box.mLX);
            assertion(root != -1, "Unexpected condition.\n");
            vSet.insert(Vector3f(
                GetXInterp(root, y, box.mZ0), (float)y, (float)box.mZ0));
        }
    }

    // Add subdivision.
    if (vSet.size() == 0)
    {
        table.Insert(Edge3(end0, end1));
        return;
    }

    float v0x = Mathf::Floor(vSet.begin()->X());
    float v1x = Mathf::Floor(vSet.rbegin()->X());
    float e0x = Mathf::Floor(table.GetPosition(end0)[0]);
    float e1x = Mathf::Floor(table.GetPosition(end1)[0]);
    if (e1x <= v0x && v1x <= e0x)
    {
        int save = end0;
        end0 = end1;
        end1 = save;
    }
    else
    {
        assertion(e0x <= v0x && v1x <= e1x, "Unexpected condition.\n");
    }

    std::set<Vector3f,Sort0>::iterator iter = vSet.begin();
    float xSave = vSet.begin()->X();
    for (++iter; iter != vSet.end(); ++iter)
    {
        assertion(xSave <= iter->X(), "Unexpected condition.\n");
        xSave = iter->X();
    }

    int v0 = table.GetNumVertices(), v1 = v0;

    // Add vertices.
    for (iter = vSet.begin(); iter != vSet.end(); ++iter)
    {
        table.Insert(*iter);
    }

    // Add edges.
    table.Insert(Edge3(end0, v1));
    ++v1;
    const int imax = (int)vSet.size() - 1;
    for (int i = 1; i <= imax; ++i, ++v0, ++v1)
    {
        table.Insert(Edge3(v0, v1));
    }
    table.Insert(Edge3(v0, end1));
}
//----------------------------------------------------------------------------
void Climb3D::GetZMaxEdgesM (const OctBox& box, unsigned int type,
    VETable& table)
{
    unsigned int faceType = 0;
    if (type & EB_XMIN_ZMAX) { faceType |= 0x01; }
    if (type & EB_XMAX_ZMAX) { faceType |= 0x02; }
    if (type & EB_YMIN_ZMAX) { faceType |= 0x04; }
    if (type & EB_YMAX_ZMAX) { faceType |= 0x08; }

    int end0 = 0, end1 = 0;
    switch (faceType)
    {
    case  0: return;
    case  3: end0 = EI_XMIN_ZMAX; end1 = EI_XMAX_ZMAX; break;
    case  5: end0 = EI_XMIN_ZMAX; end1 = EI_YMIN_ZMAX; break;
    case  6: end0 = EI_YMIN_ZMAX; end1 = EI_XMAX_ZMAX; break;
    case  9: end0 = EI_XMIN_ZMAX; end1 = EI_YMAX_ZMAX; break;
    case 10: end0 = EI_YMAX_ZMAX; end1 = EI_XMAX_ZMAX; break;
    case 12: end0 = EI_YMIN_ZMAX; end1 = EI_YMAX_ZMAX; break;
    default: assertion(false, "Unexpected case.\n");
    }

    std::set<Vector3f,Sort0> vSet;
    LinearMergeTree* merge;
    int root;

    for (int x = box.mX0 + 1; x < box.mX1; ++x)
    {
        merge = mYMerge[x][box.mZ1];
        if (merge->IsZeroEdge(box.mLY) || merge->HasZeroSubedge(box.mLY))
        {
            root = merge->GetZeroBase(box.mLY);
            assertion(root != -1, "Unexpected condition.\n");
            vSet.insert(Vector3f(
                (float)x, GetYInterp(x, root, box.mZ1), (float)box.mZ1));
        }
    }

    for (int y = box.mY0 + 1; y < box.mY1; ++y)
    {
        merge = mXMerge[y][box.mZ1];
        if (merge->IsZeroEdge(box.mLX) || merge->HasZeroSubedge(box.mLX))
        {
            root = merge->GetZeroBase(box.mLX);
            assertion(root != -1, "Unexpected condition.\n");
            vSet.insert(Vector3f(
                GetXInterp(root, y, box.mZ1), (float)y, (float)box.mZ1));
        }
    }

    // Add subdivision.
    if (vSet.size() == 0)
    {
        table.Insert(Edge3(end0, end1));
        return;
    }

    float v0x = Mathf::Floor(vSet.begin()->X());
    float v1x = Mathf::Floor(vSet.rbegin()->X());
    float e0x = Mathf::Floor(table.GetPosition(end0)[0]);
    float e1x = Mathf::Floor(table.GetPosition(end1)[0]);
    if (e1x <= v0x && v1x <= e0x)
    {
        int save = end0;
        end0 = end1;
        end1 = save;
    }
    else
    {
        assertion(e0x <= v0x && v1x <= e1x, "Unexpected condition.\n");
    }

    std::set<Vector3f,Sort0>::iterator iter = vSet.begin();
    float xSave = vSet.begin()->X();
    for (++iter; iter != vSet.end(); ++iter)
    {
        assertion(xSave <= iter->X(), "Unexpected condition.\n");
        xSave = iter->X();
    }

    int v0 = table.GetNumVertices(), v1 = v0;

    // Add vertices.
    for (iter = vSet.begin(); iter != vSet.end(); ++iter)
    {
        table.Insert(*iter);
    }

    // Add edges.
    table.Insert(Edge3(end0, v1));
    ++v1;
    const int imax = (int)vSet.size() - 1;
    for (int i = 1; i <= imax; ++i, ++v0, ++v1)
    {
        table.Insert(Edge3(v0, v1));
    }
    table.Insert(Edge3(v0, end1));
}
//----------------------------------------------------------------------------
void Climb3D::GetYMinEdgesM (const OctBox& box, unsigned int type,
    VETable& table)
{
    unsigned int faceType = 0;
    if (type & EB_XMIN_YMIN) { faceType |= 0x01; }
    if (type & EB_XMAX_YMIN) { faceType |= 0x02; }
    if (type & EB_YMIN_ZMIN) { faceType |= 0x04; }
    if (type & EB_YMIN_ZMAX) { faceType |= 0x08; }

    int end0 = 0, end1 = 0;
    switch (faceType)
    {
    case  0: return;
    case  3: end0 = EI_XMIN_YMIN; end1 = EI_XMAX_YMIN; break;
    case  5: end0 = EI_XMIN_YMIN; end1 = EI_YMIN_ZMIN; break;
    case  6: end0 = EI_YMIN_ZMIN; end1 = EI_XMAX_YMIN; break;
    case  9: end0 = EI_XMIN_YMIN; end1 = EI_YMIN_ZMAX; break;
    case 10: end0 = EI_YMIN_ZMAX; end1 = EI_XMAX_YMIN; break;
    case 12: end0 = EI_YMIN_ZMIN; end1 = EI_YMIN_ZMAX; break;
    default: assertion(false, "Unexpected case.\n");
    }

    std::set<Vector3f,Sort1> vSet;
    LinearMergeTree* merge;
    int root;

    for (int x = box.mX0 + 1; x < box.mX1; ++x)
    {
        merge = mZMerge[x][box.mY0];
        if (merge->IsZeroEdge(box.mLZ) || merge->HasZeroSubedge(box.mLZ))
        {
            root = merge->GetZeroBase(box.mLZ);
            assertion(root != -1, "Unexpected condition.\n");
            vSet.insert(Vector3f(
                (float)x, (float)box.mY0, GetZInterp(x, box.mY0, root)));
        }
    }

    for (int z = box.mZ0 + 1; z < box.mZ1; ++z)
    {
        merge = mXMerge[box.mY0][z];
        if (merge->IsZeroEdge(box.mLX) || merge->HasZeroSubedge(box.mLX))
        {
            root = merge->GetZeroBase(box.mLX);
            assertion(root != -1, "Unexpected condition.\n");
            vSet.insert(Vector3f(
                GetXInterp(root, box.mY0, z), (float)box.mY0, (float)z));
        }
    }

    // Add subdivision.
    if (vSet.size() == 0)
    {
        table.Insert(Edge3(end0, end1));
        return;
    }

    float v0z = Mathf::Floor(vSet.begin()->Z());
    float v1z = Mathf::Floor(vSet.rbegin()->Z());
    float e0z = Mathf::Floor(table.GetPosition(end0)[2]);
    float e1z = Mathf::Floor(table.GetPosition(end1)[2]);
    if (e1z <= v0z && v1z <= e0z)
    {
        int save = end0;
        end0 = end1;
        end1 = save;
    }
    else
    {
        assertion(e0z <= v0z && v1z <= e1z, "Unexpected condition.\n");
    }

    std::set<Vector3f,Sort1>::iterator iter = vSet.begin();
    float zSave = vSet.begin()->Z();
    for (++iter; iter != vSet.end(); ++iter)
    {
        assertion(zSave <= iter->Z(), "Unexpected condition.\n");
        zSave = iter->Z();
    }

    int v0 = table.GetNumVertices(), v1 = v0;

    // Add vertices.
    for (iter = vSet.begin(); iter != vSet.end(); ++iter)
    {
        table.Insert(*iter);
    }

    // Add edges.
    table.Insert(Edge3(end0, v1));
    ++v1;
    const int imax = (int)vSet.size() - 1;
    for (int i = 1; i <= imax; ++i, ++v0, ++v1)
    {
        table.Insert(Edge3(v0, v1));
    }
    table.Insert(Edge3(v0, end1));
}
//----------------------------------------------------------------------------
void Climb3D::GetYMaxEdgesM (const OctBox& box, unsigned int type,
    VETable& table)
{
    unsigned int faceType = 0;
    if (type & EB_XMIN_YMAX) { faceType |= 0x01; }
    if (type & EB_XMAX_YMAX) { faceType |= 0x02; }
    if (type & EB_YMAX_ZMIN) { faceType |= 0x04; }
    if (type & EB_YMAX_ZMAX) { faceType |= 0x08; }

    int end0 = 0, end1 = 0;
    switch (faceType)
    {
    case  0: return;
    case  3: end0 = EI_XMIN_YMAX; end1 = EI_XMAX_YMAX; break;
    case  5: end0 = EI_XMIN_YMAX; end1 = EI_YMAX_ZMIN; break;
    case  6: end0 = EI_YMAX_ZMIN; end1 = EI_XMAX_YMAX; break;
    case  9: end0 = EI_XMIN_YMAX; end1 = EI_YMAX_ZMAX; break;
    case 10: end0 = EI_YMAX_ZMAX; end1 = EI_XMAX_YMAX; break;
    case 12: end0 = EI_YMAX_ZMIN; end1 = EI_YMAX_ZMAX; break;
    default: assertion(false, "Unexpected case.\n");
    }

    std::set<Vector3f,Sort1> vSet;
    LinearMergeTree* merge;
    int root;

    for (int x = box.mX0 + 1; x < box.mX1; ++x)
    {
        merge = mZMerge[x][box.mY1];
        if (merge->IsZeroEdge(box.mLZ) || merge->HasZeroSubedge(box.mLZ))
        {
            root = merge->GetZeroBase(box.mLZ);
            assertion(root != -1, "Unexpected condition.\n");
            vSet.insert(Vector3f(
                (float)x, (float)box.mY1, GetZInterp(x, box.mY1, root)));
        }
    }

    for (int z = box.mZ0 + 1; z < box.mZ1; ++z)
    {
        merge = mXMerge[box.mY1][z];
        if (merge->IsZeroEdge(box.mLX) || merge->HasZeroSubedge(box.mLX))
        {
            root = merge->GetZeroBase(box.mLX);
            assertion(root != -1, "Unexpected condition.\n");
            vSet.insert(Vector3f(
                GetXInterp(root, box.mY1, z), (float)box.mY1, (float)z));
        }
    }

    // Add subdivision.
    if (vSet.size() == 0)
    {
        table.Insert(Edge3(end0, end1));
        return;
    }

    float v0z = Mathf::Floor(vSet.begin()->Z());
    float v1z = Mathf::Floor(vSet.rbegin()->Z());
    float e0z = Mathf::Floor(table.GetPosition(end0)[2]);
    float e1z = Mathf::Floor(table.GetPosition(end1)[2]);
    if (e1z <= v0z && v1z <= e0z)
    {
        int save = end0;
        end0 = end1;
        end1 = save;
    }
    else
    {
        assertion(e0z <= v0z && v1z <= e1z, "Unexpected condition.\n");
    }

    std::set<Vector3f,Sort1>::iterator iter = vSet.begin();
    float zSave = vSet.begin()->Z();
    for (++iter; iter != vSet.end(); ++iter)
    {
        assertion(zSave <= iter->Z(), "Unexpected condition.\n");
        zSave = iter->Z();
    }

    int v0 = table.GetNumVertices(), v1 = v0;

    // Add vertices.
    for (iter = vSet.begin(); iter != vSet.end(); ++iter)
    {
        table.Insert(*iter);
    }

    // Add edges.
    table.Insert(Edge3(end0, v1));
    ++v1;
    const int imax = (int)vSet.size() - 1;
    for (int i = 1; i <= imax; ++i, ++v0, ++v1)
    {
        table.Insert(Edge3(v0, v1));
    }
    table.Insert(Edge3(v0, end1));
}
//----------------------------------------------------------------------------
void Climb3D::GetXMinEdgesM (const OctBox& box, unsigned int type,
    VETable& table)
{
    unsigned int faceType = 0;
    if (type & EB_XMIN_YMIN) { faceType |= 0x01; }
    if (type & EB_XMIN_YMAX) { faceType |= 0x02; }
    if (type & EB_XMIN_ZMIN) { faceType |= 0x04; }
    if (type & EB_XMIN_ZMAX) { faceType |= 0x08; }

    int end0 = 0, end1 = 0;
    switch (faceType)
    {
    case  0: return;
    case  3: end0 = EI_XMIN_YMIN; end1 = EI_XMIN_YMAX; break;
    case  5: end0 = EI_XMIN_YMIN; end1 = EI_XMIN_ZMIN; break;
    case  6: end0 = EI_XMIN_ZMIN; end1 = EI_XMIN_YMAX; break;
    case  9: end0 = EI_XMIN_YMIN; end1 = EI_XMIN_ZMAX; break;
    case 10: end0 = EI_XMIN_ZMAX; end1 = EI_XMIN_YMAX; break;
    case 12: end0 = EI_XMIN_ZMIN; end1 = EI_XMIN_ZMAX; break;
    default: assertion(false, "Unexpected case.\n");
    }

    std::set<Vector3f,Sort2> vSet;
    LinearMergeTree* merge;
    int root;

    for (int z = box.mZ0 + 1; z < box.mZ1; ++z)
    {
        merge = mYMerge[box.mX0][z];
        if (merge->IsZeroEdge(box.mLY) || merge->HasZeroSubedge(box.mLY))
        {
            root = merge->GetZeroBase(box.mLY);
            assertion(root != -1, "Unexpected condition.\n");
            vSet.insert(Vector3f(
                (float)box.mX0, GetYInterp(box.mX0, root, z), (float)z));
        }
    }

    for (int y = box.mY0 + 1; y < box.mY1; ++y)
    {
        merge = mZMerge[box.mX0][y];
        if (merge->IsZeroEdge(box.mLZ) || merge->HasZeroSubedge(box.mLZ))
        {
            root = merge->GetZeroBase(box.mLZ);
            assertion(root != -1, "Unexpected condition.\n");
            vSet.insert(Vector3f(
                (float)box.mX0, (float)y, GetZInterp(box.mX0, y, root)));
        }
    }

    // Add subdivision.
    if (vSet.size() == 0)
    {
        table.Insert(Edge3(end0, end1));
        return;
    }

    float v0y = Mathf::Floor(vSet.begin()->Y());
    float v1y = Mathf::Floor(vSet.rbegin()->Y());
    float e0y = Mathf::Floor(table.GetPosition(end0)[1]);
    float e1y = Mathf::Floor(table.GetPosition(end1)[1]);
    if (e1y <= v0y && v1y <= e0y)
    {
        int save = end0;
        end0 = end1;
        end1 = save;
    }
    else
    {
        assertion(e0y <= v0y && v1y <= e1y, "Unexpected condition.\n");
    }

    std::set<Vector3f,Sort2>::iterator iter = vSet.begin();
    float ySave = vSet.begin()->Y();
    for (++iter; iter != vSet.end(); ++iter)
    {
        assertion(ySave <= iter->Y(), "Unexpected condition.\n");
        ySave = iter->Y();
    }

    int v0 = table.GetNumVertices(), v1 = v0;

    // Add vertices.
    for (iter = vSet.begin(); iter != vSet.end(); ++iter)
    {
        table.Insert(*iter);
    }

    // Add edges.
    table.Insert(Edge3(end0, v1));
    ++v1;
    const int imax = (int)vSet.size() - 1;
    for (int i = 1; i <= imax; ++i, ++v0, ++v1)
    {
        table.Insert(Edge3(v0, v1));
    }
    table.Insert(Edge3(v0, end1));
}
//----------------------------------------------------------------------------
void Climb3D::GetXMaxEdgesM (const OctBox& box, unsigned int type,
    VETable& table)
{
    unsigned int faceType = 0;
    if (type & EB_XMAX_YMIN) { faceType |= 0x01; }
    if (type & EB_XMAX_YMAX) { faceType |= 0x02; }
    if (type & EB_XMAX_ZMIN) { faceType |= 0x04; }
    if (type & EB_XMAX_ZMAX) { faceType |= 0x08; }

    int end0 = 0, end1 = 0;
    switch (faceType)
    {
    case  0: return;
    case  3: end0 = EI_XMAX_YMIN; end1 = EI_XMAX_YMAX; break;
    case  5: end0 = EI_XMAX_YMIN; end1 = EI_XMAX_ZMIN; break;
    case  6: end0 = EI_XMAX_ZMIN; end1 = EI_XMAX_YMAX; break;
    case  9: end0 = EI_XMAX_YMIN; end1 = EI_XMAX_ZMAX; break;
    case 10: end0 = EI_XMAX_ZMAX; end1 = EI_XMAX_YMAX; break;
    case 12: end0 = EI_XMAX_ZMIN; end1 = EI_XMAX_ZMAX; break;
    default: assertion(false, "Unexpected case.\n");
    }

    std::set<Vector3f,Sort2> vSet;
    LinearMergeTree* merge;
    int root;

    for (int z = box.mZ0 + 1; z < box.mZ1; ++z)
    {
        merge = mYMerge[box.mX1][z];
        if (merge->IsZeroEdge(box.mLY) || merge->HasZeroSubedge(box.mLY))
        {
            root = merge->GetZeroBase(box.mLY);
            assertion(root != -1, "Unexpected condition.\n");
            vSet.insert(Vector3f(
                (float)box.mX1, GetYInterp(box.mX1, root, z), (float)z));
        }
    }

    for (int y = box.mY0+1; y < box.mY1; y++)
    {
        merge = mZMerge[box.mX1][y];
        if (merge->IsZeroEdge(box.mLZ) || merge->HasZeroSubedge(box.mLZ))
        {
            root = merge->GetZeroBase(box.mLZ);
            assertion(root != -1, "Unexpected condition.\n");
            vSet.insert(Vector3f(
                (float)box.mX1, (float)y, GetZInterp(box.mX1, y, root)));
        }
    }

    // Add subdivision.
    if (vSet.size() == 0)
    {
        table.Insert(Edge3(end0, end1));
        return;
    }

    float v0y = Mathf::Floor(vSet.begin()->Y());
    float v1y = Mathf::Floor(vSet.rbegin()->Y());
    float e0y = Mathf::Floor(table.GetPosition(end0)[1]);
    float e1y = Mathf::Floor(table.GetPosition(end1)[1]);
    if (e1y <= v0y && v1y <= e0y)
    {
        int save = end0;
        end0 = end1;
        end1 = save;
    }
    else
    {
        assertion(e0y <= v0y && v1y <= e1y, "Unexpected condition.\n");
    }

    std::set<Vector3f,Sort2>::iterator iter = vSet.begin();
    float ySave = vSet.begin()->Y();
    for (++iter; iter != vSet.end(); ++iter)
    {
        assertion(ySave <= iter->Y(), "Unexpected condition.\n");
        ySave = iter->Y();
    }

    int v0 = table.GetNumVertices(), v1 = v0;

    // Add vertices.
    for (iter = vSet.begin(); iter != vSet.end(); ++iter)
    {
        table.Insert(*iter);
    }

    // Add edges.
    table.Insert(Edge3(end0, v1));
    ++v1;
    const int imax = (int)vSet.size() - 1;
    for (int i = 1; i <= imax; ++i, ++v0, ++v1)
    {
        table.Insert(Edge3(v0, v1));
    }
    table.Insert(Edge3(v0, end1));
}
//----------------------------------------------------------------------------
Vector3f Climb3D::GetGradient (const Vector3f& position)
{
    int x = (int)position[0];
    if (x < 0 || x >= mTwoPowerN)
    {
        return Vector3f::ZERO;
    }

    int y = (int)position[1];
    if (y < 0 || y >= mTwoPowerN)
    {
        return Vector3f::ZERO;
    }

    int z = (int)position[2];
    if (z < 0 || z >= mTwoPowerN)
    {
        return Vector3f::ZERO;
    }

    int i000 = x + mSize*(y + mSize*z);
    int i100 = i000 + 1;
    int i010 = i000 + mSize;
    int i110 = i100 + mSize;
    int i001 = i000 + mSizeSqr;
    int i101 = i100 + mSizeSqr;
    int i011 = i010 + mSizeSqr;
    int i111 = i110 + mSizeSqr;
    float f000 = mData[i000];
    float f100 = mData[i100];
    float f010 = mData[i010];
    float f110 = mData[i110];
    float f001 = mData[i001];
    float f101 = mData[i101];
    float f011 = mData[i011];
    float f111 = mData[i111];
    
    float fx = position[0] - (float)x;
    float fy = position[0] - (float)y;
    float fz = position[0] - (float)z;
    float oneMinusX = 1.0f - fx;
    float oneMinusY = 1.0f - fy;
    float oneMinusZ = 1.0f - fz;
    float tmp0, tmp1;
    Vector3f gradient;

    tmp0 = oneMinusY*(f100 - f000) + fy*(f110 - f010);
    tmp1 = oneMinusY*(f101 - f001) + fy*(f111 - f011);
    gradient[0] = oneMinusZ*tmp0 + fz*tmp1;
    
    tmp0 = oneMinusX*(f010 - f000) + fx*(f110 - f100);
    tmp1 = oneMinusX*(f011 - f001) + fx*(f111 - f101);
    gradient[1] = oneMinusZ*tmp0 + fz*tmp1;
    
    tmp0 = oneMinusX*(f001 - f000) + fx*(f101 - f100);
    tmp1 = oneMinusX*(f011 - f010) + fx*(f111 - f110);
    gradient[2] = oneMinusY*tmp0 + oneMinusY*tmp1;

    return gradient;
}
//----------------------------------------------------------------------------

