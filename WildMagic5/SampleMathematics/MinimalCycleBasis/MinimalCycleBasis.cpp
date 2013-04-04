// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "MinimalCycleBasis.h"

WM5_WINDOW_APPLICATION(MinimalCycleBasis);

#define EXTRACT_PRIMITIVES

//----------------------------------------------------------------------------
MinimalCycleBasis::MinimalCycleBasis ()
    :
    WindowApplication2("SampleMathematics/MinimalCycleBasis", 0, 0, 400, 152,
        Float4(1.0f, 1.0f, 1.0f, 1.0f))
{
    Environment::InsertDirectory(ThePath + "Data/");

    mColors = 0;
}
//----------------------------------------------------------------------------
bool MinimalCycleBasis::OnInitialize ()
{
    if (!WindowApplication2::OnInitialize())
    {
        return false;
    }

    // Load the vertices and edges.  These are from the image PlanarGraph.wmif
    // which is an example in the MinimalCycleBasis.pdf document.  The image
    // is 400-by-152, which is why the application window size was chosen as
    // it is.
    std::string path = Environment::GetPathR("PlanarGraph.txt");
    std::ifstream inFile(path.c_str());

    int numVertices;
    inFile >> numVertices;
    int i;
    for (i = 0; i < numVertices; ++i)
    {
        int x, y;
        inFile >> x;
        inFile >> y;
        y = GetHeight() - 1 - y;
        mGraph.InsertVertex(IVector2(x, y), i);
    }

    int numEdges;
    inFile >> numEdges;
    for (i = 0; i < numEdges; ++i)
    {
        int v0, v1;
        inFile >> v0;
        inFile >> v1;
        mGraph.InsertEdge(v0, v1);
    }

#ifdef EXTRACT_PRIMITIVES
    mGraph.ExtractPrimitives(mPrimitives);
    mColors = new1<ColorRGB>(mPrimitives.size());
    const int numPrimitives = (int)mPrimitives.size();
    for (i = 0; i < numPrimitives; ++i)
    {
        mColors[i].r = rand() % 256;
        mColors[i].g = rand() % 256;
        mColors[i].b = rand() % 256;
    }
#endif

    DoFlip(true);
    OnDisplay();
    return true;
}
//----------------------------------------------------------------------------
void MinimalCycleBasis::OnTerminate ()
{
    const int numPrimitives = (int)mPrimitives.size();
    for (int i = 0; i < numPrimitives; ++i)
    {
        delete0(mPrimitives[i]);
    }
    delete1(mColors);

    WindowApplication2::OnTerminate();
}
//----------------------------------------------------------------------------
void MinimalCycleBasis::OnDisplay ()
{
    ClearScreen();

#ifdef EXTRACT_PRIMITIVES
    IVector2 v0, v1;
    int x0, y0, x1, y1, j0, j1;

    const int numPrimitives = (int)mPrimitives.size();
    int i;
    for (i = 0; i < numPrimitives; ++i)
    {
        Graph::Primitive* primitive = mPrimitives[i];
        int numElements = (int)primitive->Sequence.size();
        switch (primitive->Type)
        {
        case Graph::PT_ISOLATED_VERTEX:
            v0 = primitive->Sequence[0].first;
            x0 = (int)v0.X();
            y0 = (int)v0.Y();
            SetThickPixel(x0, y0, 1, mColors[i]);
            break;
        case Graph::PT_FILAMENT:
            for (j0 = 0, j1 = 1; j1 < numElements; ++j0, ++j1)
            {
                v0 = primitive->Sequence[j0].first;
                x0 = (int)v0.X();
                y0 = (int)v0.Y();
                v1 = primitive->Sequence[j1].first;
                x1 = (int)v1.X();
                y1 = (int)v1.Y();
                DrawLine(x0, y0, x1, y1, mColors[i]);
            }
            break;
        case Graph::PT_MINIMAL_CYCLE:
            for (j0 = numElements - 1, j1 = 0; j1 < numElements; j0 = j1++)
            {
                v0 = primitive->Sequence[j0].first;
                x0 = (int)v0.X();
                y0 = (int)v0.Y();
                v1 = primitive->Sequence[j1].first;
                x1 = (int)v1.X();
                y1 = (int)v1.Y();
                DrawLine(x0, y0, x1, y1, mColors[i]);
            }
            break;
        }
    }
#else
    // Draw the edges.
    Color gray(192, 192, 192);
    const Graph::Edges& edges = mGraph.GetEdges();
    Graph::Edges::const_iterator eiter = edges.begin();
    Graph::Edges::const_iterator eend = edges.end();
    for (/**/; eiter != eend; ++eiter)
    {
        EdgeKey edge = eiter->first;
        IVector2 v0 = mGraph.GetVertex(edge.V[0])->Position;
        IVector2 v1 = mGraph.GetVertex(edge.V[1])->Position;
        int x0 = (int)v0.X();
        int y0 = (int)v0.Y();
        int x1 = (int)v1.X();
        int y1 = (int)v1.Y();
        DrawLine(x0, y0, x1, y1, gray);
    }

    // Draw the vertices.
    Color blue(0, 0, 255);
    const Graph::Vertices& vertices = mGraph.GetVertices();
    Graph::Vertices::const_iterator viter = vertices.begin();
    Graph::Vertices::const_iterator vend = vertices.end();
    for (/**/; viter != vend; ++viter)
    {
        Graph::Vertex* vertex = viter->second;
        int x = (int)vertex->Position.X();
        int y = (int)vertex->Position.Y();
        SetThickPixel(x, y, 1, blue);
    }
#endif

    WindowApplication2::OnDisplay();
}
//----------------------------------------------------------------------------
