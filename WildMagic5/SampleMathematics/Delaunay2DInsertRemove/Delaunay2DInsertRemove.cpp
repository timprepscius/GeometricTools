// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Delaunay2DInsertRemove.h"

WM5_WINDOW_APPLICATION(Delaunay2DInsertRemove);

//----------------------------------------------------------------------------
Delaunay2DInsertRemove::Delaunay2DInsertRemove ()
    :
    WindowApplication2("SampleMathematics/Delaunay2DInsertRemove", 0, 0, 512,
        512, Float4(1.0f, 1.0f, 1.0f, 1.0f))
{
    mDelaunay = 0;
    mNumVertices = 0;
    mVertices = 0;
    mNumTriangles = 0;
    mIndices = 0;
    mAdjacencies = 0;

    mSize = GetWidth();
}
//----------------------------------------------------------------------------
bool Delaunay2DInsertRemove::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    // Generate random points and triangulate.
    mNumVertices = 32;
    mVertices = new1<Vector2f>(mNumVertices);
    int i;
    for (i = 0; i < mNumVertices; ++i)
    {
        mVertices[i].X() = mSize*Mathf::IntervalRandom(0.125f, 0.875f);
        mVertices[i].Y() = mSize*Mathf::IntervalRandom(0.125f, 0.875f);
    }

    mDelaunay = new0 IncrementalDelaunay2f(0.0f, 0.0f, (float)mSize,
        (float)mSize);

    for (i = 0; i < mNumVertices; ++i)
    {
        mDelaunay->Insert(mVertices[i]);
    }

    GetMesh();

    DoFlip(true);
    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void Delaunay2DInsertRemove::OnTerminate ()
{
    delete0(mDelaunay);
    delete1(mVertices);
    delete1(mIndices);
    delete1(mAdjacencies);

    WindowApplication2::OnTerminate();
}
//----------------------------------------------------------------------------
void Delaunay2DInsertRemove::OnDisplay ()
{
    ClearScreen();

    ColorRGB gray(128, 128, 128);
    ColorRGB blue(0, 0, 255);
    int i, x0, y0, x1, y1, x2, y2;
    Vector2f v0, v1, v2;
    const std::vector<Vector2f>& vertices = mDelaunay->GetVertices();

    // Draw the triangle mesh.
    std::set<Vector2f> used;
    for (i = 0; i < mNumTriangles; ++i)
    {
        v0 = vertices[mIndices[3*i]];
        x0 = (int)(v0.X() + 0.5f);
        y0 = (int)(v0.Y() + 0.5f);

        v1 = vertices[mIndices[3*i+1]];
        x1 = (int)(v1.X() + 0.5f);
        y1 = (int)(v1.Y() + 0.5f);

        v2 = vertices[mIndices[3*i+2]];
        x2 = (int)(v2.X() + 0.5f);
        y2 = (int)(v2.Y() + 0.5f);

        DrawLine(x0, y0, x1, y1, gray);
        DrawLine(x1, y1, x2, y2, gray);
        DrawLine(x2, y2, x0, y0, gray);

        used.insert(v0);
        used.insert(v1);
        used.insert(v2);
    }

    // Draw the vertices.
    std::set<Vector2f>::iterator iter = used.begin();
    std::set<Vector2f>::iterator end = used.end();
    for (/**/; iter != end; ++iter)
    {
        v0 = *iter;
        x0 = (int)(v0.X() + 0.5f);
        y0 = (int)(v0.Y() + 0.5f);
        SetThickPixel(x0, y0, 2, blue);
    }

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
bool Delaunay2DInsertRemove::OnMouseClick (int button, int state, int x,
    int y, unsigned int modifiers)
{
    Vector2f pos((float)x, (float)(mSize - 1 - y));

    if (button == MOUSE_LEFT_BUTTON && state == MOUSE_DOWN)
    {
        if (modifiers & KEY_SHIFT)
        {
            // Remove a point from the triangulation.
            int i = mDelaunay->GetContainingTriangle(pos);
            if (i >= 0)
            {
                float bary[3];
                mDelaunay->GetBarycentricSet(i, pos, bary);

                int indices[3];
                mDelaunay->GetIndexSet(i, indices);

                float maxBary = bary[0];
                int maxIndex = 0;
                if (bary[1] > maxBary)
                {
                    maxBary = bary[1];
                    maxIndex = 1;
                }
                if (bary[2] > maxBary)
                {
                    maxBary = bary[2];
                    maxIndex = 2;
                }

                pos = mDelaunay->GetVertices()[indices[maxIndex]];
                mDelaunay->Remove(pos);
                GetMesh();
                OnDisplay();
            }
        }
        else
        {
            // Insert a point into the triangulation.
            mDelaunay->Insert(pos);
            GetMesh();
            OnDisplay();
        }
    }

    return true;
}
//----------------------------------------------------------------------------
void Delaunay2DInsertRemove::GetMesh ()
{
    mDelaunay->GenerateRepresentation();

    mNumTriangles = mDelaunay->GetNumTriangles();
    delete1(mIndices);
    mIndices = new1<int>(3*mNumTriangles);
    size_t numBytes = 3*mNumTriangles*sizeof(int);
    memcpy(mIndices, mDelaunay->GetIndices(), numBytes);
    delete1(mAdjacencies);
    mAdjacencies = new1<int>(3*mNumTriangles);
    memcpy(mAdjacencies, mDelaunay->GetAdjacencies(), numBytes);
}
//----------------------------------------------------------------------------
