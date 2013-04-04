// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "Boolean2D.h"

WM5_WINDOW_APPLICATION(Boolean2D);

//----------------------------------------------------------------------------
Boolean2D::Boolean2D ()
    :
    WindowApplication2("SampleMathematics/Boolean2D", 0, 0, 256, 256,
        Float4(1.0f, 1.0f, 1.0f, 1.0f))
{
    mActive = 0;
    mPoly0 = 0;
    mPoly1 = 0;
    mSize = GetWidth();
}
//----------------------------------------------------------------------------
bool Boolean2D::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    mChoice = 0;
    mPoly0 = ConstructInvertedEll();
    mPoly1 = ConstructPentagon();
    DoBoolean();

    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void Boolean2D::OnTerminate ()
{
    delete0(mPoly0);
    delete0(mPoly1);

    WindowApplication2::OnTerminate();
}
//----------------------------------------------------------------------------
void Boolean2D::OnDisplay ()
{
    ClearScreen();

    DrawPolySolid(*mPoly0, ColorRGB(255, 0, 0));
    DrawPolySolid(*mPoly1, ColorRGB(0, 255, 0));
    if (mActive)
    {
        DrawPolySolid(*mActive, ColorRGB(0, 0, 255));
    }

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
bool Boolean2D::OnKeyDown (unsigned char key, int x, int y)
{
    if (WindowApplication2::OnKeyDown(key, x, y))
    {
        return true;
    }

    switch (key)
    {
    case 'n':
    case 'N':
        delete0(mPoly0);
        delete0(mPoly1);
        mActive = 0;

        mChoice = (mChoice + 1) % 3;
        switch (mChoice)
        {
        case 0:
            mPoly0 = ConstructInvertedEll();
            mPoly1 = ConstructPentagon();
            break;
        case 1:
            mPoly0 = ConstructSquare();
            mPoly1 = ConstructSShape();
            break;
        case 2:
            mPoly0 = ConstructPolyWithHoles();
            mPoly1 = ConstructPentagon();
            break;
        }
        DoBoolean();
        break;

    case 'p':
    case 'P':
        mActive = 0;
        break;
    case 'u':
    case 'U':
        mActive = &mUnion;
        break;
    case 'i':
    case 'I':
        mActive = &mIntersection;
        break;
    case 'd':
    case 'D':
        mActive = &mDiff01;
        break;
    case 'e':
    case 'E':
        mActive = &mDiff10;
        break;
    case 'x':
    case 'X':
        mActive = &mXor;
        break;
    }

    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
BspPolygon2* Boolean2D::ConstructInvertedEll ()
{
    double w = (double)GetWidth();
    double d1d8 = 0.125*w;
    double d2d8 = 0.250*w;
    double d3d8 = 0.375*w;
    double d5d8 = 0.625*w;
    double d6d8 = 0.750*w;
    double d7d8 = 0.875*w;

    const int numVertices = 10;
    Vector2d vertices[numVertices] =
    {
        Vector2d(d1d8, d1d8),
        Vector2d(d3d8, d1d8),
        Vector2d(d3d8, d3d8),
        Vector2d(d2d8, d3d8),
        Vector2d(d2d8, d6d8),
        Vector2d(d5d8, d6d8),
        Vector2d(d5d8, d5d8),
        Vector2d(d7d8, d5d8),
        Vector2d(d7d8, d7d8),
        Vector2d(d1d8, d7d8)
    };

    BspPolygon2* poly = new0 BspPolygon2();
    for (int i0 = numVertices - 1, i1 = 0; i1 < numVertices; i0 = i1++)
    {
        poly->InsertVertex(vertices[i1]);
        poly->InsertEdge(Edge2(i0, i1));
    }
    poly->Finalize();
    return poly;
}
//----------------------------------------------------------------------------
BspPolygon2* Boolean2D::ConstructPentagon ()
{
    const int numVertices = 5;

    double primitiveAngle = Mathd::TWO_PI/numVertices;
    double radius = 0.35*GetWidth();
    double cx = 0.5*GetWidth(), cy = 0.5*GetWidth();

    Vector2d vertices[numVertices];
    for (int i = 0; i < numVertices; ++i)
    {
        double angle = i*primitiveAngle;
        vertices[i].X() = cx + radius*Mathd::Cos(angle);
        vertices[i].Y() = cy + radius*Mathd::Sin(angle);
    }

    BspPolygon2* poly = new0 BspPolygon2();
    for (int i0 = numVertices - 1, i1 = 0; i1 < numVertices; i0 = i1++)
    {
        poly->InsertVertex(vertices[i1]);
        poly->InsertEdge(Edge2(i0, i1));
    }
    poly->Finalize();
    return poly;
}
//----------------------------------------------------------------------------
BspPolygon2* Boolean2D::ConstructSquare ()
{
    double w = (double)GetWidth();
    double d2d8 = 0.250*w;
    double d6d8 = 0.750*w;

    const int numVertices = 4;
    Vector2d vertices[numVertices] =
    {
        Vector2d(d2d8, d2d8),
        Vector2d(d6d8, d2d8),
        Vector2d(d6d8, d6d8),
        Vector2d(d2d8, d6d8)
    };

    BspPolygon2* poly = new0 BspPolygon2();
    for (int i0 = numVertices - 1, i1 = 0; i1 < numVertices; i0 = i1++)
    {
        poly->InsertVertex(vertices[i1]);
        poly->InsertEdge(Edge2(i0, i1));
    }
    poly->Finalize();
    return poly;
}
//----------------------------------------------------------------------------
BspPolygon2* Boolean2D::ConstructSShape ()
{
    double w = (double)GetWidth();
    double d10d32 = 10.0*w/32.0;
    double d12d32 = 12.0*w/32.0;
    double d13d32 = 13.0*w/32.0;
    double d16d32 = 16.0*w/32.0;
    double d19d32 = 19.0*w/32.0;
    double d20d32 = 20.0*w/32.0;
    double d22d32 = 22.0*w/32.0;
    double d24d32 = 24.0*w/32.0;
    double d26d32 = 26.0*w/32.0;
    double d28d32 = 28.0*w/32.0;

    const int numVertices = 12;
    Vector2d vertices[numVertices] =
    {
        Vector2d(d24d32, d10d32),
        Vector2d(d28d32, d10d32),
        Vector2d(d28d32, d16d32),
        Vector2d(d22d32, d16d32),
        Vector2d(d22d32, d19d32),
        Vector2d(d24d32, d19d32),
        Vector2d(d24d32, d22d32),
        Vector2d(d20d32, d22d32),
        Vector2d(d20d32, d13d32),
        Vector2d(d26d32, d13d32),
        Vector2d(d26d32, d12d32),
        Vector2d(d24d32, d12d32)
    };

    BspPolygon2* poly = new0 BspPolygon2();
    for (int i0 = numVertices - 1, i1 = 0; i1 < numVertices; i0 = i1++)
    {
        poly->InsertVertex(vertices[i1]);
        poly->InsertEdge(Edge2(i0, i1));
    }
    poly->Finalize();
    return poly;
}
//----------------------------------------------------------------------------
BspPolygon2* Boolean2D::ConstructPolyWithHoles ()
{
    double w = (double)GetWidth();
    double d2d16 = 2.0*w/16.0;
    double d3d16 = 3.0*w/16.0;
    double d4d16 = 4.0*w/16.0;
    double d6d16 = 6.0*w/16.0;
    double d14d16 = 14.0*w/16.0;

    const int numVertices = 6;
    Vector2d vertices[numVertices] =
    {
        // outer boundary
        Vector2d(d2d16, d2d16),
        Vector2d(d14d16, d2d16),
        Vector2d(d2d16, d14d16),

        // inner boundary
        Vector2d(d4d16, d3d16),
        Vector2d(d6d16, d6d16),
        Vector2d(d6d16, d3d16)
    };

    BspPolygon2* poly = new0 BspPolygon2();
    for (int i = 0; i < numVertices; ++i)
    {
        poly->InsertVertex(vertices[i]);
    }

    poly->InsertEdge(Edge2(0, 1));
    poly->InsertEdge(Edge2(1, 2));
    poly->InsertEdge(Edge2(2, 0));
    poly->InsertEdge(Edge2(3, 4));
    poly->InsertEdge(Edge2(4, 5));
    poly->InsertEdge(Edge2(5, 3));

    poly->Finalize();
    return poly;
}
//----------------------------------------------------------------------------
void Boolean2D::DrawPolySolid (BspPolygon2& polygon, ColorRGB color)
{
    Vector2d  v0, v1;
    Edge2 edge;
    int i, x0, y0, x1, y1;

    // Draw the edges.
    for (i = 0; i < polygon.GetNumEdges(); ++i)
    {
        polygon.GetEdge(i, edge);
        polygon.GetVertex(edge.I0, v0);
        polygon.GetVertex(edge.I1, v1);
        
        x0 = (int)(v0.X() + 0.5f);
        y0 = GetWidth() - 1 - (int)(v0.Y() + 0.5f);
        x1 = (int)(v1.X() + 0.5f);
        y1 = GetWidth() - 1 - (int)(v1.Y() + 0.5f);

        DrawLine(x0, y0, x1, y1, color);
    }

    // Draw the vertices.
    ColorRGB black(0, 0, 0);
    for (i = 0; i < polygon.GetNumVertices(); ++i)
    {
        polygon.GetVertex(i, v0);
        x0 = (int)(v0.X() + 0.5f);
        y0 = GetWidth() - 1 - (int)(v0.Y() + 0.5f);
        SetThickPixel(x0, y0, 1, black);
    }
}
//----------------------------------------------------------------------------
void Boolean2D::DoBoolean ()
{
    BspPolygon2& P = *mPoly0;
    BspPolygon2& Q = *mPoly1;

    mIntersection = P & Q;
    mUnion        = P | Q;
    mDiff01       = P - Q;
    mDiff10       = Q - P;
    mXor          = P ^ Q;
}
//----------------------------------------------------------------------------
