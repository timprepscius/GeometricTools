// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "PolygonDistance.h"

WM5_WINDOW_APPLICATION(PolygonDistance);

//----------------------------------------------------------------------------
PolygonDistance::PolygonDistance ()
    :
    WindowApplication2("SamplePhysics/PolygonDistance", 0, 0, 512, 512,
        Float4(1.0f, 1.0f, 1.0f, 1.0f))
{
    mSize = GetWidth();
}
//----------------------------------------------------------------------------
bool PolygonDistance::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    // Warm up the random number generator.
    srand((unsigned int)time(0));
    int i, j = rand() % 100;
    for (i = 0; i < j; ++i)
    {
        rand();
    }

    for (i = 0; i < NUM_POLYGONS; ++i)
    {
        mPolygons[i].NumVertices = 5 - i;
        mPolygons[i].Vertices = new1<Vector2f>(mPolygons[i].NumVertices);
        mPolygons[i].Polars = new1<Vector2f>(mPolygons[i].NumVertices);
        mPolygons[i].Faces = new1<Tuple<2,int> >(mPolygons[i].NumVertices);
    }

    InitialConfiguration();
    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void PolygonDistance::OnTerminate ()
{
    for (int i = 0; i < NUM_POLYGONS; ++i)
    {
        delete1(mPolygons[i].Vertices);
        delete1(mPolygons[i].Polars);
        delete1(mPolygons[i].Faces);
    }

    WindowApplication2::OnTerminate();
}
//----------------------------------------------------------------------------
void PolygonDistance::OnDisplay ()
{
    ClearScreen();

    const int lineThick = 0;
    const int solutionThick = 2;
    const int centroidThick = 4;
    ColorRGB lineColor(0, 0, 0);
    ColorRGB centroidColor(0, 0, 128);

    // Draw the polygons.
    int i;
    for (i = 0; i < NUM_POLYGONS; ++i)
    {
        const int n = mPolygons[i].NumVertices;
        for (int j0 = n-1, j1 = 0; j1 < mPolygons[i].NumVertices; j0 = j1++)
        {
            DrawLineSegment(lineThick, lineColor, mPolygons[i].Vertices[j0],
                mPolygons[i].Vertices[j1]);
        }
    }

    // Draw the segment joining the nearest points.
    for (int k0 = NUM_POLYGONS-1, k1 = 0; k1 < NUM_POLYGONS; k0 = k1++)
    {
        // Copy the polygon vertices.
        Vector2f* v00 = new1<Vector2f>(mPolygons[k0].NumVertices);
        Vector2f* v01 = new1<Vector2f>(mPolygons[k1].NumVertices);
        memcpy(v00, mPolygons[k0].Vertices,
            mPolygons[k0].NumVertices*sizeof(Vector2f));
        memcpy(v01, mPolygons[k1].Vertices,
            mPolygons[k1].NumVertices*sizeof(Vector2f));

        int statusCode;
        float retValue;
        Vector2f closest[2];
        LCPPolyDist2(mPolygons[k0].NumVertices, v00,
            mPolygons[k0].NumVertices, mPolygons[k0].Faces,
            mPolygons[k1].NumVertices, v01, mPolygons[k1].NumVertices,
            mPolygons[k1].Faces, statusCode, retValue, closest);

        // Draw the segment joining the closest points.
        DrawLineSegment(lineThick, lineColor, closest[0], closest[1]);

        if (mDrawPerpendiculars && retValue > 0.1f)
        {
            // Compute perpendiculars to edges at solution points.
            Vector2f end[2];
            ComputePerpendiculars(mPolygons[k0].NumVertices,
                mPolygons[k0].Vertices, closest[0], end);
            DrawPerpendiculars(end);
            ComputePerpendiculars(mPolygons[k1].NumVertices,
                mPolygons[k1].Vertices, closest[1], end);
            DrawPerpendiculars(end);
        }

        // Draw the nearest points.
        switch (statusCode)
        {
        case LCPPolyDist2::SC_FOUND_SOLUTION:
        {
            ColorRGB solutionFound(0, 128, 0);
            for (i = 0; i < 2; ++i)
            {
                DrawPoints(solutionThick, solutionFound, closest[i]);
            }
            break;
        }
        case LCPPolyDist2::SC_TEST_POINTS_TEST_FAILED:
        {
            ColorRGB testPointsFailed(0, 0, 128);
            for (i = 0; i < 2; ++i)
            {
                DrawPoints(solutionThick, testPointsFailed, closest[i]);
            }
            break;
        }
        case LCPPolyDist2::SC_VERIFY_FAILURE:
        {
            ColorRGB verifyFailed(128, 0, 0);
            for (i = 0; i < 2; ++i)
            {
                DrawPoints(solutionThick, verifyFailed, closest[i]);
            }
            break;
        }
        }

        // Draw the centroids.
        DrawPoints(centroidThick, centroidColor, mPolygons[k0].Centroid);
        DrawPoints(centroidThick, centroidColor, mPolygons[k1].Centroid);

        delete1(v00);
        delete1(v01);
    }

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
bool PolygonDistance::OnKeyDown (unsigned char key, int x, int y)
{
    if (WindowApplication2::OnKeyDown(key, x, y))
    {
        return true;
    }

    switch (key)
    {
    case 'g':
        NextConfiguration();
        OnDisplay();
        return true;
    case '0':
        InitialConfiguration();
        OnDisplay();
        return true;
    case 'p':  // toggle for drawing the perpendiculars
        mDrawPerpendiculars = !mDrawPerpendiculars;
        OnDisplay();
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void PolygonDistance::InitialConfiguration ()
{
    // first polygon
    mPolygons[0].Vertices[0] = Vector2f(50.0f, 50.0f);
    mPolygons[0].Vertices[1] = Vector2f(60.0f, 200.0f);
    mPolygons[0].Vertices[2] = Vector2f(90.0f, 250.0f);
    mPolygons[0].Vertices[3] = Vector2f(150.0f, 300.0f);
    mPolygons[0].Vertices[4] = Vector2f(200.0f, 100.0f);

    GetPolarRepresentation(mPolygons[0].NumVertices, mPolygons[0].Vertices,
        mPolygons[0].Centroid, mPolygons[0].Polars);
    
    mPolygons[0].Faces[0][0] = 0;
    mPolygons[0].Faces[0][1] = 1;
    mPolygons[0].Faces[1][0] = 1;
    mPolygons[0].Faces[1][1] = 2;
    mPolygons[0].Faces[2][0] = 2;
    mPolygons[0].Faces[2][1] = 3;
    mPolygons[0].Faces[3][0] = 3;
    mPolygons[0].Faces[3][1] = 4;
    mPolygons[0].Faces[4][0] = 4;
    mPolygons[0].Faces[4][1] = 0;

    // second polygon
    mPolygons[1].Vertices[0] = Vector2f(250.0f, 250.0f);
    mPolygons[1].Vertices[1] = Vector2f(260.0f, 400.0f);
    mPolygons[1].Vertices[2] = Vector2f(350.0f, 450.0f);
    mPolygons[1].Vertices[3] = Vector2f(375.0f, 300.0f);

    GetPolarRepresentation(mPolygons[1].NumVertices, mPolygons[1].Vertices,
        mPolygons[1].Centroid, mPolygons[1].Polars);
    
    mPolygons[1].Faces[0][0] = 0;
    mPolygons[1].Faces[0][1] = 1;
    mPolygons[1].Faces[1][0] = 1;
    mPolygons[1].Faces[1][1] = 2;
    mPolygons[1].Faces[2][0] = 2;
    mPolygons[1].Faces[2][1] = 3;
    mPolygons[1].Faces[3][0] = 3;
    mPolygons[1].Faces[3][1] = 0;

    // third polygon
    mPolygons[2].Vertices[0] = Vector2f(200.0f, 200.0f);
    mPolygons[2].Vertices[1] = Vector2f(400.0f, 300.0f);
    mPolygons[2].Vertices[2] = Vector2f(350.0f, 100.0f);

    GetPolarRepresentation(mPolygons[2].NumVertices, mPolygons[2].Vertices,
        mPolygons[2].Centroid, mPolygons[2].Polars);
    
    mPolygons[2].Faces[0][0] = 0;
    mPolygons[2].Faces[0][1] = 1;
    mPolygons[2].Faces[1][0] = 1;
    mPolygons[2].Faces[1][1] = 2;
    mPolygons[2].Faces[2][0] = 2;
    mPolygons[2].Faces[2][1] = 0;

    for (int i = 0; i < NUM_POLYGONS; ++i)
    {
        // Randomly select a direction to rotate.
        mPolygons[i].Sign = (Mathf::SymmetricRandom() > 0.0f ? 1 : -1);
    }

    mDrawPerpendiculars = false;
}
//----------------------------------------------------------------------------
void PolygonDistance::NextConfiguration ()
{
    for (int i = 0; i < NUM_POLYGONS; ++i)
    {
        RotatePolygon(mPolygons[i].NumVertices, mPolygons[i].Sign,
            mPolygons[i].Polars);

        GetCartesianRepresentation(mPolygons[i].NumVertices,
            mPolygons[i].Vertices, mPolygons[i].Centroid,
            mPolygons[i].Polars);
    }
}
//----------------------------------------------------------------------------
void PolygonDistance::GetPolarRepresentation (int numVertices,
    const Vector2f* vertices, Vector2f& centroid, Vector2f* polars)
{
    centroid = Vector2f(0.0f, 0.0f);
    int i;
    for (i = 0; i < numVertices; ++i)
    {
        centroid += vertices[i];
    }
    centroid /= (float)numVertices;

    for (i = 0; i < numVertices; ++i)
    {
        Vector2f diff = vertices[i] - centroid;
        float temp;
        if (diff[0] > 0.0f)
        {
            temp = Mathf::ATan(diff[1]/diff[0]);
        }
        else if (diff[0] < 0.0f)
        {
            temp = Mathf::ATan(diff[1]/diff[0]) + Mathf::PI;
        }
        else
        {
            temp = Mathf::HALF_PI;
        }
        polars[i][0] = diff.Length();
        polars[i][1] = temp;
    }
}
//----------------------------------------------------------------------------
void PolygonDistance::GetCartesianRepresentation (int numVertices,
    Vector2f* vertices, const Vector2f& centroid, const Vector2f* polars)
{
    for (int i = 0; i < numVertices; ++i)
    {
        vertices[i][0] = polars[i][0]*Mathf::Cos(polars[i][1]);
        vertices[i][1] = polars[i][0]*Mathf::Sin(polars[i][1]);
        vertices[i] += centroid;
    }
}
//----------------------------------------------------------------------------
void PolygonDistance::RotatePolygon (int numVertices, int sign,
    Vector2f* polars)
{
    // Rotate figures by random amounts in randomly selected directions.
    const float randomWidth = 0.08f;
    float factor = sign*randomWidth;
    for (int i = 0; i < numVertices; ++i)
    {
        polars[i][1] += factor;
    }
}
//----------------------------------------------------------------------------
void PolygonDistance::ComputePerpendiculars (int numVertices,
    const Vector2f* vertices, const Vector2f& closest, Vector2f end[2])
{
    const float normLength = 40.0f;
    const float close = 0.1f;
    end[0] = closest;
    end[1] = closest;
    for (int i = 0; i < numVertices; ++i)
    {
        int j = (i + 1) % numVertices;
        float denom = vertices[i][0] - vertices[j][0];
        if (denom == 0.0f)
        {
            // The edge is vertical.
            if (closest[1] == vertices[i][0])
            {
                // The result lies on the edge.
                Vector2f diff0 = closest - vertices[0];
                Vector2f diff1 = closest - vertices[1];
                if (diff0.Length() > close && diff1.Length() > close)
                {
                    // The result is on the edge but not a vertex.
                    end[0] = closest + normLength*Vector2f::UNIT_X;
                    end[1] = closest - normLength*Vector2f::UNIT_X;
                }
                return;
            }
            else
            {
                // The result is not on this edge, go to next edge.
                continue;
            }
        }

        // The edge is not vertical.
        float numer = closest[0] - vertices[j][0];
        float t = numer/denom;
        if (t <= 0.0f || t >= 1.0f)
        {
            // The result is not in this line segment.
            continue;
        }

        float temp = Mathf::FAbs(t*vertices[i][1] + (1.0f - t)*vertices[j][1]
            - closest[1]);
        if (temp < close)
        {
            // The solution is on this edge.
            Vector2f diff2 = closest - vertices[0];
            Vector2f diff3 = closest - vertices[1];
            if (diff2.Length() > close && diff3.Length() > close)
            {
                // The result is on the edge but not a vertex.
                Vector2f norm = Vector2f(vertices[i][1] - vertices[j][1],
                    -denom);
                norm.Normalize();
                end[0] = closest + normLength*norm;
                end[1] = closest - normLength*norm;
                return;
            }
        }
    }
}
//----------------------------------------------------------------------------
void PolygonDistance::DrawPerpendiculars (const Vector2f end[2])
{
    DrawLineSegment(1, ColorRGB(0, 128, 0), end[0], end[1]);
}
//----------------------------------------------------------------------------
void PolygonDistance::DrawLineSegment (int thick, ColorRGB color,
    const Vector2f& end0, const Vector2f& end1)
{
    const int imax = 2048;
    for (int i = 0; i <= imax; ++i)
    {
        float t = i/(float)imax;
        Vector2f position = t*end0 + (1.0f - t)*end1;
        int x = (int)(position.X() + 0.5f);
        int y = mSize - 1 - (int)(position.Y() + 0.5f);
        for (int dy = -thick; dy <= thick; ++dy)
        {
            for (int dx = -thick; dx <= thick; ++dx)
            {
                SetPixel(x + dx, y + dy, color);
            }
        }
    }
}
//----------------------------------------------------------------------------
void PolygonDistance::DrawPoints (int thick, ColorRGB color,
    const Vector2f& point)
{
    int x = (int)(point.X() + 0.5f);
    int y = mSize - 1 - (int)(point.Y() + 0.5f);
    for (int dy = -thick; dy <= thick; ++dy)
    {
        for (int dx = -thick; dx <= thick; ++dx)
        {
            SetPixel(x + dx, y + dy, color);
        }
    }
}
//----------------------------------------------------------------------------
