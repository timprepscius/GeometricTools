// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "ClodPolyline.h"

WM5_WINDOW_APPLICATION(ClodPolyline);

//----------------------------------------------------------------------------
ClodPolyline::ClodPolyline ()
    :
    WindowApplication2("SampleMathematics/ClodPolyline", 0, 0, 256, 256,
        Float4(1.0f, 1.0f, 1.0f, 1.0f))
{
    mPolyline = 0;
    mSize = GetWidth();
}
//----------------------------------------------------------------------------
bool ClodPolyline::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    // Generate points on unit circle, then adjust the distances to center.
    int numVertices = 16;
    Vector3f* vertices = new1<Vector3f>(numVertices);
    int i;
    for (i = 0; i < numVertices; ++i)
    {
        float angle = Mathf::TWO_PI*i/numVertices;
        vertices[i].X() = Mathf::Cos(angle);
        vertices[i].Y() = Mathf::Sin(angle);
        vertices[i].Z() = 0.0f;

        float adjust = 1.0f + 0.25f*Mathf::SymmetricRandom();
        vertices[i] *= adjust;
    }

    mPolyline = new0 Polyline3(numVertices, vertices, true);

    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void ClodPolyline::OnTerminate ()
{
    delete0(mPolyline);

    WindowApplication2::OnTerminate();
}
//----------------------------------------------------------------------------
void ClodPolyline::OnDisplay ()
{
    ClearScreen();

    ColorRGB black(0, 0, 0);
    const int numVertices = mPolyline->GetNumVertices();
    const Vector3f* vertices = mPolyline->GetVertices();
    const int numEdges = mPolyline->GetNumEdges();
    const int* edges = mPolyline->GetEdges();

    Vector3f vertex;
    int i;
    for (i = 0; i < numVertices; ++i)
    {
        vertex = vertices[i];
        int x = (int)(0.25f*mSize*(vertex.X() + 2.0f));
        int y = mSize - 1 - (int)(0.25f*mSize*(vertex.Y() + 2.0f));
        SetThickPixel(x, y, 1, black);
    }

    for (i = 0; i < numEdges; ++i)
    {
        vertex = vertices[edges[2*i]];
        int x0 = (int)(0.25f*mSize*(vertex.X() + 2.0f));
        int y0 = mSize - 1 - (int)(0.25f*mSize*(vertex.Y() + 2.0f));

        vertex = vertices[edges[2*i+1]];
        int x1 = (int)(0.25*mSize*(vertex.X() + 2.0f));
        int y1 = mSize - 1 - (int)(0.25f*mSize*(vertex.Y() + 2.0f));
        DrawLine(x0, y0, x1, y1, black);
    }

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
bool ClodPolyline::OnKeyDown (unsigned char key, int x, int y)
{
    if (WindowApplication2::OnKeyDown(key, x, y))
    {
        return true;
    }

    int levelOfDetail;

    switch (key)
    {
    case '+':  // increase level of detail
    case '=':
        levelOfDetail = mPolyline->GetLevelOfDetail();
        if (levelOfDetail < mPolyline->GetMaxLevelOfDetail())
        {
            mPolyline->SetLevelOfDetail(levelOfDetail + 1);
            OnDisplay();
        }
        return true;
    case '-':  // decrease level of detail
    case '_':
        levelOfDetail = mPolyline->GetLevelOfDetail();
        if (levelOfDetail > mPolyline->GetMinLevelOfDetail())
        {
            mPolyline->SetLevelOfDetail(levelOfDetail - 1);
            OnDisplay();
        }
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
