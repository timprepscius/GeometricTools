// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.5.2 (2012/07/06)

#include "VETable.h"
#include "Wm5Assert.h"

//----------------------------------------------------------------------------
VETable::VETable ()
    :
    mVertices(18)
{
}
//----------------------------------------------------------------------------
VETable::~VETable ()
{
}
//----------------------------------------------------------------------------
bool VETable::IsValidVertex (int i) const
{
    assertion(0 <= i && i < (int)mVertices.size(), "Invalid index.\n");
    return mVertices[i].mValid;
}
//----------------------------------------------------------------------------
int VETable::GetNumVertices () const
{
    return (int)mVertices.size();
}
//----------------------------------------------------------------------------
const Vector3f& VETable::GetPosition (int i) const
{
    assertion(0 <= i && i < (int)mVertices.size(), "Invalid index.\n");
    return mVertices[i].mPosition;
}
//----------------------------------------------------------------------------
void VETable::Insert (int i, float x, float y, float z)
{
    assertion(0 <= i && i < 18, "Invalid index.\n");
    Vertex& vertex = mVertices[i];
    vertex.mPosition = Vector3f(x, y, z);
    vertex.mValid = true;
}
//----------------------------------------------------------------------------
void VETable::Insert (const Vector3f& position)
{
    mVertices.push_back(Vertex(position));
}
//----------------------------------------------------------------------------
void VETable::Insert (const Edge3& edge)
{
    const int numVertices = (int)mVertices.size();
    WM5_UNUSED(numVertices);
    assertion(
        0 <= edge.first && edge.first < numVertices &&
        0 <= edge.second && edge.second < numVertices,
        "Invalid edge.\n");

    Vertex& vertex0 = mVertices[edge.first];
    Vertex& vertex1 = mVertices[edge.second];

    assertion(vertex0.mAdjQuantity < 4 && vertex1.mAdjQuantity < 4,
        "Invalid adjacent quantity.\n");

    vertex0.mAdjacent[vertex0.mAdjQuantity] = edge.second;
    ++vertex0.mAdjQuantity;
    vertex1.mAdjacent[vertex1.mAdjQuantity] = edge.first;
    ++vertex1.mAdjQuantity;
}
//----------------------------------------------------------------------------
void VETable::RemoveTrianglesEC (std::vector<Vector3f>& positions,
    std::vector<TriangleKey>& triangles)
{
    // Ear-clip the wireframe to get the triangles.
    TriangleKey triangle;
    while (RemoveEC(triangle))
    {
        const int v0 = (int)positions.size(), v1 = v0 + 1, v2 = v1 + 1;
        triangles.push_back(TriangleKey(v0, v1, v2));
        positions.push_back(mVertices[triangle.V[0]].mPosition);
        positions.push_back(mVertices[triangle.V[1]].mPosition);
        positions.push_back(mVertices[triangle.V[2]].mPosition);
    }
}
//----------------------------------------------------------------------------
void VETable::RemoveTrianglesSE (std::vector<Vector3f>& positions,
    std::vector<TriangleKey>& triangles)
{
    const int vmax = (int)mVertices.size();
    assertion(vmax > 18, "Invalid number of vertices.\n");

    // Compute centroid of vertices.
    Vector3f centroid(0.0f, 0.0f, 0.0f);
    int i, quantity = 0;
    for (i = 0; i < vmax; i++)
    {
        Vertex& vertex = mVertices[i];
        if (vertex.mValid)
        {
            assertion(vertex.mAdjQuantity == 2,
                "Invalid adjacent quantity.\n");
            centroid += vertex.mPosition;
            ++quantity;
        }
    }
    centroid /= (float)quantity;

    const int v0 = (int)positions.size();
    positions.push_back(centroid);

    int i1 = 18;
    int v1 = v0 + 1;
    positions.push_back(mVertices[i1].mPosition);

    int i2 = mVertices[i1].mAdjacent[1], v2;
    for (i = 0; i < quantity - 1; ++i)
    {
        v2 = v1 + 1;
        positions.push_back(mVertices[i2].mPosition);
        triangles.push_back(TriangleKey(v0, v1, v2));
        if (mVertices[i2].mAdjacent[1] != i1)
        {
            i1 = i2;
            i2 = mVertices[i2].mAdjacent[1];
        }
        else
        {
            i1 = i2;
            i2 = mVertices[i2].mAdjacent[0];
        }
        v1 = v2;
    }

    assertion(i2 == 18, "Invalid index.\n");
    v2 = v0 + 1;
    triangles.push_back(TriangleKey(v0, v1, v2));
}
//----------------------------------------------------------------------------
void VETable::RemoveVertex (int i)
{
    assertion(0 <= i && i < (int)mVertices.size(), "Invalid index.\n");
    Vertex& vertex0 = mVertices[i];

    assertion(vertex0.mAdjQuantity == 2, "Invalid adjacent quantity.\n");

    int a0 = vertex0.mAdjacent[0], a1 = vertex0.mAdjacent[1];
    Vertex& adjVertex0 = mVertices[a0];
    Vertex& adjVertex1 = mVertices[a1];

    int j;
    for (j = 0; j < adjVertex0.mAdjQuantity; j++)
    {
        if (adjVertex0.mAdjacent[j] == i)
        {
            adjVertex0.mAdjacent[j] = a1;
            break;
        }
    }
    assertion(j != adjVertex0.mAdjQuantity, "Unexpected condition.\n");

    for (j = 0; j < adjVertex1.mAdjQuantity; j++)
    {
        if ( adjVertex1.mAdjacent[j] == i )
        {
            adjVertex1.mAdjacent[j] = a0;
            break;
        }
    }
    assertion(j != adjVertex1.mAdjQuantity, "Unexpected condition.\n");

    vertex0.mValid = false;

    if (adjVertex0.mAdjQuantity == 2)
    {
        if (adjVertex0.mAdjacent[0] == adjVertex0.mAdjacent[1])
        {
            adjVertex0.mValid = false;
        }
    }

    if (adjVertex1.mAdjQuantity == 2)
    {
        if (adjVertex1.mAdjacent[0] == adjVertex1.mAdjacent[1])
        {
            adjVertex1.mValid = false;
        }
    }
}
//----------------------------------------------------------------------------
bool VETable::RemoveEC (TriangleKey& triangle)
{
    const int numVertices = (int)mVertices.size();
    for (int i = 0; i < numVertices; ++i)
    {
        Vertex& vertex = mVertices[i];
        if (vertex.mValid && vertex.mAdjQuantity == 2)
        {
            triangle.V[0] = i;
            triangle.V[1] = vertex.mAdjacent[0];
            triangle.V[2] = vertex.mAdjacent[1];
            RemoveVertex(i);
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// VETable::Vertex
//----------------------------------------------------------------------------
VETable::Vertex::Vertex ()
    :
    mAdjQuantity(0),
    mValid(false)
{
}
//----------------------------------------------------------------------------
VETable::Vertex::Vertex (const Vector3f& vertex)
    :
    mPosition(vertex),
    mAdjQuantity(0),
    mValid(true)
{
}
//----------------------------------------------------------------------------
