// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Delaunay2D.h"

WM5_WINDOW_APPLICATION(Delaunay2D);

//----------------------------------------------------------------------------
Delaunay2D::Delaunay2D ()
    :
    WindowApplication2("SampleMathematics/Delaunay2D", 0, 0, 512, 512,
        Float4(1.0f, 1.0f, 1.0f, 1.0f))
{
    mDelaunay1 = 0;
    mDelaunay2 = 0;
    mNumVertices = 0;
    mVertices = 0;
    mNumTriangles = 0;
    mIndices = 0;
    mAdjacencies = 0;
    mCurrentTriX = -1;
    mCurrentTriY = -1;
    mCurrentIndex = 0;
    mSize = GetWidth();
}
//----------------------------------------------------------------------------
bool Delaunay2D::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    // Generate random points and triangulate.
    mNumVertices = 256;
    mVertices = new1<Vector2f>(mNumVertices);
    for (int i = 0; i < mNumVertices; ++i)
    {
        mVertices[i].X() = mSize*Mathf::IntervalRandom(0.125f, 0.875f);
        mVertices[i].Y() = mSize*Mathf::IntervalRandom(0.125f, 0.875f);

        // Comment out the last two lines and uncomment these to see
        // how the Delaunay code handles degeneracies in dimension.
        //mVertices[i].X() = mSize*Mathf::IntervalRandom(0.125f, 0.875f);
        //mVertices[i].Y() = 2.1f*mVertices[i].X();
    }

    mDelaunay2 = new0 Delaunay2f(mNumVertices, mVertices, 0.001f, false,
        Query::QT_REAL);

    if (mDelaunay2->GetDimension() == 2)
    {
        mNumTriangles = mDelaunay2->GetNumSimplices();
        mIndices = new1<int>(3*mNumTriangles);
        size_t numBytes = 3*mNumTriangles*sizeof(int);
        memcpy(mIndices, mDelaunay2->GetIndices(), numBytes);
        mAdjacencies = new1<int>(3*mNumTriangles);
        memcpy(mAdjacencies, mDelaunay2->GetAdjacencies(), numBytes);

#ifdef _DEBUG
        // If H is the number of hull edges and N is the number of hull
        // vertices, then the triangulation must have 2*N-2-H triangles
        // and 3*N-3-H edges.
        int numEdges = 0;
        int* indices = 0;
        mDelaunay2->GetHull(numEdges, indices);
        int numUniqueVertices = mDelaunay2->GetNumUniqueVertices();
        int numTrianglesTheoretical = 2*numUniqueVertices - 2 - numEdges;
        assertion(numTrianglesTheoretical == mNumTriangles,
            "Triangle count is not theoretically correct.\n");
        int numEdgesTheoretical = 3*numUniqueVertices - 3 - numEdges;
        WM5_UNUSED(numEdgesTheoretical);
        delete1(indices);
#endif
    }
    else
    {
        mDelaunay1 = mDelaunay2->GetDelaunay1();
        delete0(mDelaunay2);
        mDelaunay2 = 0;
    }

    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void Delaunay2D::OnTerminate ()
{
    delete0(mDelaunay1);
    delete0(mDelaunay2);
    delete1(mVertices);
    delete1(mIndices);
    delete1(mAdjacencies);

    WindowApplication2::OnTerminate();
}
//----------------------------------------------------------------------------
void Delaunay2D::OnDisplay ()
{
    ClearScreen();

    ColorRGB white(255, 255, 255);
    ColorRGB lightGray(192, 192, 192);
    ColorRGB gray(128, 128, 128);
    ColorRGB red(255, 0, 0);
    ColorRGB blue(0, 0, 255);
    ColorRGB green(0, 255, 0);
    ColorRGB black(0, 0, 0);

    int i, x0, y0, x1, y1, x2, y2;
    Vector2f v0, v1, v2;

    if (mDelaunay2)
    {
        // Draw the triangle mesh.
        for (i = 0; i < mNumTriangles; ++i)
        {
            v0 = mVertices[mIndices[3*i]];
            x0 = (int)(v0.X() + 0.5f);
            y0 = (int)(v0.Y() + 0.5f);

            v1 = mVertices[mIndices[3*i + 1]];
            x1 = (int)(v1.X() + 0.5f);
            y1 = (int)(v1.Y() + 0.5f);

            v2 = mVertices[mIndices[3*i + 2]];
            x2 = (int)(v2.X() + 0.5f);
            y2 = (int)(v2.Y() + 0.5f);

            DrawLine(x0, y0, x1, y1, gray);
            DrawLine(x1, y1, x2, y2, gray);
            DrawLine(x2, y2, x0, y0, gray);
        }

        // Draw the hull.
        int numEdges = 0;
        int* edges = 0;
        mDelaunay2->GetHull(numEdges, edges);
        for (i = 0; i < numEdges; ++i)
        {
            v0 = mVertices[edges[2*i]];
            x0 = (int)(v0.X() + 0.5f);
            y0 = (int)(v0.Y() + 0.5f);

            v1 = mVertices[edges[2*i + 1]];
            x1 = (int)(v1.X() + 0.5f);
            y1 = (int)(v1.Y() + 0.5f);

            DrawLine(x0, y0, x1, y1, red);
        }
        delete1(edges);


        // Draw the search path.
        for (i = 0; i <= mDelaunay2->GetPathLast(); ++i)
        {
            int index = mDelaunay2->GetPath()[i];

            v0 = mVertices[mIndices[3*index]];
            v1 = mVertices[mIndices[3*index + 1]];
            v2 = mVertices[mIndices[3*index + 2]];

            Vector2f center = (v0 + v1 + v2)/3.0f;
            int x = (int)(center.X() + 0.5f);
            int y = (int)(center.Y() + 0.5f);
            if (i < mDelaunay2->GetPathLast())
            {
                Fill(x, y, blue, white);
            }
            else
            {
                Fill(x, y, red, white);
            }
        }

        if (mCurrentTriX >= 0)
        {
            // Draw the current triangle.
            Fill(mCurrentTriX, mCurrentTriY, green, red);
        }
        else
        {
            // Draw the last edge when the selected point is outside the hull.
            int lastV0, lastV1, lastVOpposite;
            mDelaunay2->GetLastEdge(lastV0, lastV1, lastVOpposite);

            v0 = mVertices[lastV0];
            x0 = (int)(v0.X() + 0.5f);
            y0 = (int)(v0.Y() + 0.5f);

            v1 = mVertices[lastV1];
            x1 = (int)(v1.X() + 0.5f);
            y1 = (int)(v1.Y() + 0.5f);

            DrawLine(x0, y0, x1, y1, black);
        }
    }
    else
    {
        const int* indices = mDelaunay1->GetIndices();
        for (i = 0; i+1 < mNumVertices; ++i)
        {
            v0 = mVertices[indices[i]];
            x0 = (int)(v0.X() + 0.5f);
            y0 = (int)(v0.Y() + 0.5f);

            v1 = mVertices[indices[i + 1]];
            x1 = (int)(v1.X() + 0.5f);
            y1 = (int)(v1.Y() + 0.5f);

            DrawLine(x0, y0, x1, y1, lightGray);
        }

        for (i = 0; i < mNumVertices; ++i)
        {
            assertion(0 <= indices[i] && indices[i] < mNumVertices,
                "Invalid index into vertex array.\n");

            v0 = mVertices[indices[i]];
            x0 = (int)(v0.X() + 0.5f);
            y0 = (int)(v0.Y() + 0.5f);

            float w = i/(float)(mNumVertices - 1);
            float omw = 1.0f - 2;
            unsigned char r = (unsigned char)(omw*64.0f + w*255.0f);
            unsigned char b = (unsigned char)(omw*255.0f + w*64.0f);
            SetThickPixel(x0, y0, 1, ColorRGB(r, 0, b));
        }
    }

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
bool Delaunay2D::OnMouseClick (int button, int state, int x, int y,
    unsigned int)
{
    if (!mDelaunay2 || button != MOUSE_LEFT_BUTTON)
    {
        return false;
    }

    if (state == MOUSE_DOWN)
    {
        Vector2f pos((float)x, (float)y);
        int i = mDelaunay2->GetContainingTriangle(pos);
        if (i >= 0)
        {
            mCurrentTriX = x;
            mCurrentTriY = y;
        }
        else
        {
            mCurrentTriX = -1;
            mCurrentTriY = -1;
        }
        OnDisplay();
    }

    return true;
}
//----------------------------------------------------------------------------
