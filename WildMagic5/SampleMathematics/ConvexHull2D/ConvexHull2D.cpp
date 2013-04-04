// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "ConvexHull2D.h"

WM5_WINDOW_APPLICATION(ConvexHull2D);

const int g_iSize = 512;

//----------------------------------------------------------------------------
ConvexHull2D::ConvexHull2D ()
    :
    WindowApplication2("SampleMathematics/ConvexHull2D", 0, 0, 512, 512,
        Float4(1.0f, 1.0f, 1.0f, 1.0f)),
    mEpsilon(0.001f),
    mTextColor(0.0f, 0.0f, 0.0f, 1.0f)
{
    mQueryType = Query::QT_REAL;
    mHull = 0;
    mNumVertices = 0;
    mVertices = 0;
    mSize = GetWidth();
}
//----------------------------------------------------------------------------
bool ConvexHull2D::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    GenerateHull2D();
    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void ConvexHull2D::OnTerminate ()
{
    delete0(mHull);
    delete1(mVertices);

    WindowApplication2::OnTerminate();
}
//----------------------------------------------------------------------------
void ConvexHull2D::OnDisplay ()
{
    ClearScreen();

    ColorRGB black(0,0,0), gray(128,128,128), blue(0,0,255);

    int dimension = mHull->GetDimension();
    int numSimplices = mHull->GetNumSimplices();
    const int* indices = mHull->GetIndices();

    int i0, i1, x0, y0, x1, y1;
    Vector2f v0, v1;

    if (dimension == 0)
    {
        // draw point
        v0 = mVertices[0];
        x0 = UnitToScreen(v0.X());
        y0 = UnitToScreen(v0.Y());
        SetPixel(x0, y0, gray);
    }
    else if (dimension == 1)
    {
        // draw line segment
        v0 = mVertices[indices[0]];
        x0 = UnitToScreen(v0.X());
        y0 = UnitToScreen(v0.Y());

        v1 = mVertices[indices[1]];
        x1 = UnitToScreen(v1.X());
        y1 = UnitToScreen(v1.Y());

        DrawLine(x0, y0, x1, y1, gray);
    }
    else
    {
        // draw convex polygon
        for (i0 = numSimplices - 1, i1 = 0; i1 < numSimplices; i0 = i1++)
        {
            v0 = mVertices[indices[i0]];
            x0 = UnitToScreen(v0.X());
            y0 = UnitToScreen(v0.Y());

            v1 = mVertices[indices[i1]];
            x1 = UnitToScreen(v1.X());
            y1 = UnitToScreen(v1.Y());

            DrawLine(x0, y0, x1, y1, gray);
        }
    }

    // draw input points
    for (i0 = 0; i0 < mNumVertices; ++i0)
    {
        v0 = mVertices[i0];
        x0 = UnitToScreen(v0.X());
        y0 = UnitToScreen(v0.Y());
        SetThickPixel(x0, y0, 1, blue);
    }

    // draw hull vertices
    if (indices)
    {
        for (i0 = 0; i0 < numSimplices; ++i0)
        {
            v0 = mVertices[indices[i0]];
            x0 = UnitToScreen(v0.X());
            y0 = UnitToScreen(v0.Y());
            SetThickPixel(x0, y0, 1, black);
        }
    }
    else
    {
        v0 = mVertices[0];
        x0 = UnitToScreen(v0.X());
        y0 = UnitToScreen(v0.Y());
        SetThickPixel(x0, y0, 1, black);
    }

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
void ConvexHull2D::ScreenOverlay ()
{
    char message[512];

    switch (mQueryType)
    {
    case Query::QT_INT64:
        strcpy(message, "query type = INT64");
        break;
    case Query::QT_INTEGER:
        strcpy(message, "query type = INTEGER");
        break;
    case Query::QT_RATIONAL:
        strcpy(message, "query type = RATIONAL");
        break;
    case Query::QT_REAL:
        strcpy(message, "query type = REAL");
        break;
    case Query::QT_FILTERED:
        strcpy(message, "query type = FILTERED");
        break;
    }

    mRenderer->Draw(8, 16, mTextColor, message);
}
//----------------------------------------------------------------------------
bool ConvexHull2D::OnKeyDown (unsigned char key, int x, int y)
{
    if (WindowApplication2::OnKeyDown(key, x, y))
    {
        return true;
    }

    switch (key)
    {
    // Generate points that are nearly the same point.
    case '0':
        GenerateHull0D();
        OnDisplay();
        return true;

    // Generate points that are nearly collinear.
    case '1':
        GenerateHull1D();
        OnDisplay();
        return true;

    // Generate points that have a convex polygon hull.
    case '2':
        GenerateHull2D();
        OnDisplay();
        return true;

    // Lots of collinear points that lead to a convex polygon hull.
    case 'l':
    case 'L':
        GenerateHullManyCollinear();
        OnDisplay();
        return true;

    // query type INT64
    case 'n':
    case 'N':
        mQueryType = Query::QT_INT64;
        RegenerateHull();
        OnDisplay();
        return true;

    // query type INTEGER
    case 'i':
    case 'I':
        mQueryType = Query::QT_INTEGER;
        RegenerateHull();
        OnDisplay();
        return true;

    // query type RATIONAL
    case 'r':
    case 'R':
        mQueryType = Query::QT_RATIONAL;
        RegenerateHull();
        OnDisplay();
        return true;

    // query type REAL (float)
    case 'f':
    case 'F':
        mQueryType = Query::QT_REAL;
        RegenerateHull();
        OnDisplay();
        return true;

    // query type FILTERED
    case 'c':
    case 'C':
        mQueryType = Query::QT_FILTERED;
        RegenerateHull();
        OnDisplay();
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void ConvexHull2D::GenerateHull0D ()
{
    mNumVertices = 8;
    delete1(mVertices);
    mVertices = new1<Vector2f>(mNumVertices);
    mVertices[0].X() = Mathf::UnitRandom();
    mVertices[0].Y() = Mathf::UnitRandom();
    for (int i = 1; i < mNumVertices; ++i)
    {
        float sign = (Mathf::SymmetricRandom() > 0.0f ? 1.0f : -1.0f);
        mVertices[i].X() = mVertices[0].X() + sign*0.00001f;
        mVertices[i].Y() = mVertices[0].Y() + sign*0.00001f;
    }

    RegenerateHull();
    assertion(mHull->GetDimension() == 0, "Incorrect dimension.\n");
}
//----------------------------------------------------------------------------
void ConvexHull2D::GenerateHull1D ()
{
    mNumVertices = 32;
    delete1(mVertices);
    mVertices = new1<Vector2f>(mNumVertices);
    int qm1 = mNumVertices - 1;
    mVertices[0].X() = Mathf::UnitRandom();
    mVertices[0].Y() = Mathf::UnitRandom();
    mVertices[qm1].X() = Mathf::UnitRandom();
    mVertices[qm1].Y() = Mathf::UnitRandom();
    for (int i = 1; i < qm1; ++i)
    {
        float sign = (Mathf::SymmetricRandom() > 0.0f ? 1.0f : -1.0f);
        float t = Mathf::UnitRandom();
        mVertices[i] = (1.0f - t)*mVertices[0] + t*mVertices[qm1];
        mVertices[i].X() += sign*0.00001f;
        mVertices[i].Y() += sign*0.00001f;
    }

    RegenerateHull();
    assertion(mHull->GetDimension() == 1, "Incorrect dimension.\n");
}
//----------------------------------------------------------------------------
void ConvexHull2D::GenerateHull2D ()
{
    // Generate random points.
    mNumVertices = 256;
    delete1(mVertices);
    mVertices = new1<Vector2f>(mNumVertices);
    for (int i = 0; i < mNumVertices; ++i)
    {
        mVertices[i].X() = Mathf::UnitRandom();
        mVertices[i].Y() = Mathf::UnitRandom();
    }

    RegenerateHull();
    assertion(mHull->GetDimension() == 2, "Incorrect dimension.\n");
}
//----------------------------------------------------------------------------
void ConvexHull2D::GenerateHullManyCollinear ()
{
    // Generate a lot of nearly collinear points.
    mNumVertices = 128;
    delete1(mVertices);
    mVertices = new1<Vector2f>(mNumVertices);

    Vector2f center(0.5f, 0.5f);
    Vector2f U0(Mathf::SymmetricRandom(), Mathf::SymmetricRandom());
    U0.Normalize();
    Vector2f U1 = U0.Perp();
    float e0 = 0.5f*Mathf::UnitRandom();
    float e1 = 0.5f*Mathf::UnitRandom();

    float t;
    int i;
    for (i = 0; i < mNumVertices/4; ++i)
    {
        t = i/(mNumVertices/4.0f);
        mVertices[i] =
            center - e0*U0 - e1*U1 + 2.0f*e0*t*U0;
    }
    for (i = 0; i < mNumVertices/4; i++)
    {
        t = i/(mNumVertices/4.0f);
        mVertices[i + mNumVertices/4] =
            center + e0*U0 - e1*U1 + 2.0f*e1*t*U1;
    }
    for (i = 0; i < mNumVertices/4; i++)
    {
        t = i/(mNumVertices/4.0f);
        mVertices[i + mNumVertices/2] =
            center + e0*U0 + e1*U1 - 2.0f*e0*t*U0;
    }
    for (i = 0; i < mNumVertices/4; i++)
    {
        t = i/(mNumVertices/4.0f);
        mVertices[i + 3*mNumVertices/4] =
            center - e0*U0 + e1*U1 - 2.0f*e1*t*U1;
    }

    RegenerateHull();
    assertion(mHull->GetDimension() == 2, "Incorrect dimension.\n");
}
//----------------------------------------------------------------------------
void ConvexHull2D::RegenerateHull ()
{
    delete0(mHull);
    mHull = new0 ConvexHull2f(mNumVertices, mVertices, mEpsilon, false,
        mQueryType);

    if (mHull->GetDimension() == 1)
    {
        ConvexHull2f* save = (ConvexHull2f*)mHull;
        mHull = save->GetConvexHull1();
        delete0(save);
    }
}
//----------------------------------------------------------------------------
int ConvexHull2D::UnitToScreen (float value)
{
    return (int)(mSize*(0.25f + 0.5f*value));
}
//----------------------------------------------------------------------------
