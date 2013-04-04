// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.1.0 (2010/04/14)

#include "Castle.h"

//----------------------------------------------------------------------------
TriMesh* Castle::LoadMeshPNT1 (const std::string& name)
{
    // Get the vertex format.
    VertexFormat* vformat = VertexFormat::Create(3,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_NORMAL, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);
    int vstride = vformat->GetStride();

    // Get the positions.
    std::string filename = Environment::GetPathR(name);
    std::ifstream inFile(filename.c_str());
    int numPositions;
    Float3* positions;
    GetFloat3(inFile, numPositions, positions);

    // Get the normals.
    int numNormals;
    Float3* normals;
    GetFloat3(inFile, numNormals, normals);

    // Get the texture coordinates.
    int numTCoords;
    Float2* tcoords;
    GetFloat2(inFile, numTCoords, tcoords);

    // Get the vertices and indices.
    int numTriangles;
    inFile >> numTriangles;
    VertexPNT1* vertices = new1<VertexPNT1>(3*numTriangles);
    std::vector<VertexPNT1> PNT1Array;
    std::map<VertexPNT1,int> PNT1Map;
    std::vector<int> indices;
    for (int t = 0; t < numTriangles; ++t)
    {
        for (int j = 0, k = 3*t; j < 3; ++j, ++k)
        {
            VertexPNT1& vertex = vertices[k];
            inFile >> vertex.PIndex;
            inFile >> vertex.NIndex;
            inFile >> vertex.TIndex;

            std::map<VertexPNT1,int>::iterator miter = PNT1Map.find(vertex);
            int index;
            if (miter != PNT1Map.end())
            {
                // Second or later time the vertex is encountered.
                index = miter->second;
            }
            else
            {
                // First time the vertex is encountered.
                index = (int)PNT1Array.size();
                PNT1Map.insert(std::make_pair(vertex, index));
                PNT1Array.push_back(vertex);
            }
            indices.push_back(index);
        }
    }
    inFile.close();

    // Build the mesh.
    int numVertices = (int)PNT1Array.size();
    VertexBuffer* vbuffer = new0 VertexBuffer(numVertices, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);
    for (int i = 0; i < numVertices; ++i)
    {
        VertexPNT1& vertex = PNT1Array[i];
        vba.Position<Float3>(i) = positions[vertex.PIndex];
        vba.Normal<Float3>(i) = normals[vertex.NIndex];
        vba.TCoord<Float2>(0, i) = tcoords[vertex.TIndex];
    }

    int numIndices = (int)indices.size();
    IndexBuffer* ibuffer = new0 IndexBuffer(numIndices, sizeof(int));
    memcpy(ibuffer->GetData(), &indices[0], numIndices*sizeof(int));

    delete1(vertices);
    delete1(tcoords);
    delete1(normals);
    delete1(positions);

    return new0 TriMesh(vformat, vbuffer, ibuffer);
}
//----------------------------------------------------------------------------
TriMesh* Castle::LoadMeshPNT2 (const std::string& name)
{
    // Get the vertex format.
    VertexFormat* vformat = VertexFormat::Create(4,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_NORMAL, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 1);
    int vstride = vformat->GetStride();

    // Get the positions.
    std::string filename = Environment::GetPathR(name);
    std::ifstream inFile(filename.c_str());
    int numPositions;
    Float3* positions;
    GetFloat3(inFile, numPositions, positions);

    // Get the normals.
    int numNormals;
    Float3* normals;
    GetFloat3(inFile, numNormals, normals);

    // Get the texture coordinates for unit 0.
    int numTCoords0;
    Float2* tcoords0;
    GetFloat2(inFile, numTCoords0, tcoords0);

    // Get the texture coordinates for unit 1.
    int numTCoords1;
    Float2* tcoords1;
    GetFloat2(inFile, numTCoords1, tcoords1);

    // Get the vertices and indices.
    int numTriangles;
    inFile >> numTriangles;
    VertexPNT2* vertices = new1<VertexPNT2>(3*numTriangles);
    std::vector<VertexPNT2> PNT2Array;
    std::map<VertexPNT2,int> PNT2Map;
    std::vector<int> indices;
    for (int t = 0; t < numTriangles; ++t)
    {
        for (int j = 0, k = 3*t; j < 3; ++j, ++k)
        {
            VertexPNT2& vertex = vertices[k];
            inFile >> vertex.PIndex;
            inFile >> vertex.NIndex;
            inFile >> vertex.T0Index;
            inFile >> vertex.T1Index;

            std::map<VertexPNT2,int>::iterator miter = PNT2Map.find(vertex);
            int index;
            if (miter != PNT2Map.end())
            {
                // Second or later time the vertex is encountered.
                index = miter->second;
            }
            else
            {
                // First time the vertex is encountered.
                index = (int)PNT2Array.size();
                PNT2Map.insert(std::make_pair(vertex, index));
                PNT2Array.push_back(vertex);
            }
            indices.push_back(index);
        }
    }
    inFile.close();

    // Build the mesh.
    int numVertices = (int)PNT2Array.size();
    VertexBuffer* vbuffer = new0 VertexBuffer(numVertices, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);
    for (int i = 0; i < numVertices; ++i)
    {
        VertexPNT2& vertex = PNT2Array[i];
        vba.Position<Float3>(i) = positions[vertex.PIndex];
        vba.Normal<Float3>(i) = normals[vertex.NIndex];
        vba.TCoord<Float2>(0, i) = tcoords0[vertex.T0Index];
        vba.TCoord<Float2>(1, i) = tcoords1[vertex.T1Index];
    }

    int numIndices = (int)indices.size();
    IndexBuffer* ibuffer = new0 IndexBuffer(numIndices, sizeof(int));
    memcpy(ibuffer->GetData(), &indices[0], numIndices*sizeof(int));

    delete1(vertices);
    delete1(tcoords1);
    delete1(tcoords0);
    delete1(normals);
    delete1(positions);

    return new0 TriMesh(vformat, vbuffer, ibuffer);
}
//----------------------------------------------------------------------------
std::vector<TriMesh*> Castle::LoadMeshPNT1Multi (const std::string& name)
{
    // Get the vertex format.
    VertexFormat* vformat = VertexFormat::Create(3,
        VertexFormat::AU_POSITION, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_NORMAL, VertexFormat::AT_FLOAT3, 0,
        VertexFormat::AU_TEXCOORD, VertexFormat::AT_FLOAT2, 0);
    int vstride = vformat->GetStride();

    // Get the positions.
    std::string filename = Environment::GetPathR(name);
    std::ifstream inFile(filename.c_str());
    int numPositions;
    Float3* positions;
    GetFloat3(inFile, numPositions, positions);

    // Get the normals.
    int numNormals;
    Float3* normals;
    GetFloat3(inFile, numNormals, normals);

    // Get the texture coordinates.
    int numTCoords;
    Float2* tcoords;
    GetFloat2(inFile, numTCoords, tcoords);

    // Get the vertices and indices.
    int numMeshes;
    inFile >> numMeshes;
    std::vector<int> numTriangles(numMeshes);
    int numTotalTriangles = 0;
    int m;
    for (m = 0; m < numMeshes; ++m)
    {
        inFile >> numTriangles[m];
        numTotalTriangles += numTriangles[m];
    }

    std::vector<std::vector<int> >indices(numMeshes);
    VertexPNT1* vertices = new1<VertexPNT1>(3*numTotalTriangles);
    std::vector<VertexPNT1> PNT1Array;
    std::map<VertexPNT1,int> PNT1Map;
    for (m = 0; m < numMeshes; ++m)
    {
        for (int t = 0; t < numTriangles[m]; ++t)
        {
            for (int j = 0, k = 3*t; j < 3; ++j, ++k)
            {
                VertexPNT1& vertex = vertices[k];
                inFile >> vertex.PIndex;
                inFile >> vertex.NIndex;
                inFile >> vertex.TIndex;

                std::map<VertexPNT1,int>::iterator miter =
                    PNT1Map.find(vertex);
                int index;
                if (miter != PNT1Map.end())
                {
                    // Second or later time the vertex is encountered.
                    index = miter->second;
                }
                else
                {
                    // First time the vertex is encountered.
                    index = (int)PNT1Array.size();
                    PNT1Map.insert(std::make_pair(vertex, index));
                    PNT1Array.push_back(vertex);
                }
                indices[m].push_back(index);
            }
        }
    }
    inFile.close();

    // Build the meshes.
    int numVertices = (int)PNT1Array.size();
    VertexBuffer* vbuffer = new0 VertexBuffer(numVertices, vstride);
    VertexBufferAccessor vba(vformat, vbuffer);
    for (int i = 0; i < numVertices; ++i)
    {
        VertexPNT1& vertex = PNT1Array[i];
        vba.Position<Float3>(i) = positions[vertex.PIndex];
        vba.Normal<Float3>(i) = normals[vertex.NIndex];
        vba.TCoord<Float2>(0, i) = tcoords[vertex.TIndex];
    }

    std::vector<TriMesh*> meshes(numMeshes);
    for (m = 0; m < numMeshes; ++m)
    {
        int numIndices = (int)indices[m].size();
        IndexBuffer* ibuffer = new0 IndexBuffer(numIndices, sizeof(int));
        memcpy(ibuffer->GetData(), &indices[m][0], numIndices*sizeof(int));
        meshes[m] = new0 TriMesh(vformat, vbuffer, ibuffer);
    }

    delete1(vertices);
    delete1(tcoords);
    delete1(normals);
    delete1(positions);

    return meshes;
}
//----------------------------------------------------------------------------
void Castle::GetFloat3 (std::ifstream& inFile, int& numElements,
    Float3*& elements)
{
    inFile >> numElements;
    elements = new1<Float3>(numElements);
    for (int i = 0; i < numElements; ++i)
    {
        inFile >> elements[i][0];
        inFile >> elements[i][1];
        inFile >> elements[i][2];
    }
}
//----------------------------------------------------------------------------
void Castle::GetFloat2 (std::ifstream& inFile, int& numElements,
    Float2*& elements)
{
    inFile >> numElements;
    elements = new1<Float2>(numElements);
    for (int i = 0; i < numElements; ++i)
    {
        inFile >> elements[i][0];
        inFile >> elements[i][1];
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// VertexPNT1
//----------------------------------------------------------------------------
Castle::VertexPNT1::VertexPNT1 ()
    :
    PIndex(-1),
    NIndex(-1),
    TIndex(-1)
{
}
//----------------------------------------------------------------------------
bool Castle::VertexPNT1::operator< (const VertexPNT1& vertex) const
{
    if (PIndex < vertex.PIndex) { return true;  }
    if (PIndex > vertex.PIndex) { return false; }
    if (NIndex < vertex.NIndex) { return true;  }
    if (NIndex > vertex.NIndex) { return false; }
    return TIndex < vertex.TIndex;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// VertexPNT1
//----------------------------------------------------------------------------
Castle::VertexPNT2::VertexPNT2 ()
    :
    PIndex(-1),
    NIndex(-1),
    T0Index(-1),
    T1Index(-1)
{
}
//----------------------------------------------------------------------------
bool Castle::VertexPNT2::operator< (const VertexPNT2& vertex) const
{
    if (PIndex  < vertex.PIndex ) { return true;  }
    if (PIndex  > vertex.PIndex ) { return false; }
    if (NIndex  < vertex.NIndex ) { return true;  }
    if (NIndex  > vertex.NIndex ) { return false; }
    if (T0Index < vertex.T0Index) { return true;  }
    if (T0Index > vertex.T0Index) { return false; }
    return T1Index < vertex.T1Index;
}
//----------------------------------------------------------------------------
