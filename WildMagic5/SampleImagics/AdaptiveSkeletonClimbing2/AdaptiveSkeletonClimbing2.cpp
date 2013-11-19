// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.5.1 (2011/07/26)

#include "AdaptiveSkeletonClimbing2.h"
#include "Climb2D.h"

WM5_CONSOLE_APPLICATION(AdaptiveSkeletonClimbing2);

int AdaptiveSkeletonClimbing2::msData[81] =
{
    +1, -1, -1, +1, -1, -1, +1, -1, +1,
    +1, +1, +1, +1, +1, +1, +1, +1, +1,
    -1, -1, +1, +1, +1, +1, -1, -1, -1,
    -1, -1, +1, +1, +1, +1, -1, -1, -1,
    -1, -1, +1, +1, +1, +1, -1, -1, -1,
    +1, -1, -1, -1, -1, -1, -1, +1, +1,
    -1, -1, +1, -1, -1, -1, -1, -1, +1,
    +1, +1, +1, -1, -1, -1, +1, -1, -1,
    -1, +1, -1, +1, +1, +1, -1, +1, +1
};

ImageRGB82D* AdaptiveSkeletonClimbing2::msColor = 0;

//----------------------------------------------------------------------------
AdaptiveSkeletonClimbing2::AdaptiveSkeletonClimbing2 ()
    :
    ConsoleApplication("SampleImagics/AdaptiveSkeletonClimbing2")
{
    msColor = new0 ImageRGB82D(257, 257);
}
//----------------------------------------------------------------------------
AdaptiveSkeletonClimbing2::~AdaptiveSkeletonClimbing2 ()
{
    delete0(msColor);
}
//----------------------------------------------------------------------------
void AdaptiveSkeletonClimbing2::Test0 ()
{
    int* data = new1<int>(81);
    memcpy(data, msData, 81*sizeof(int));

    Climb2D climb(3, data);

    int numVertices, numEdges;
    Vector2f* vertices = 0;
    Climb2D::Edge2* edges = 0;
    climb.ExtractContour(0.0f, -1, numVertices, vertices, numEdges, edges);
    climb.MakeUnique(numVertices, vertices, numEdges, edges);

    std::ofstream outFile("vedata0.txt");
    int i;
    for (i = 0; i < numVertices; ++i)
    {
        Vector2f& vertex = vertices[i];
        outFile << i << " " << vertex[0] << " , " << vertex[1] << std::endl;
    }
    outFile << std::endl;

    for (i = 0; i < numEdges; ++i)
    {
        Climb2D::Edge2& edge = edges[i];
        outFile << i << " " << edge.first << " " << edge.second << std::endl;
    }

    delete1(vertices);
    delete1(edges);
}
//----------------------------------------------------------------------------
void AdaptiveSkeletonClimbing2::Test1 ()
{
    ImageInt2D image(257, 257);
    image = (Eint)0;

    int x, y;
    for (y = 32; y < 224; ++y)
    {
        for (x = 64; x < 192; ++x)
        {
            image(x, y) = 100;
        }
    }

    ImageInt2D blur(257, 257);
    blur = (Eint)0;

    int i;
    for (i = 1; i <= 8; ++i)
    {
        for (y = 0; y < 257; ++y)
        {
            for (x = 0; x < 257; ++x)
            {
                if (image(x, y) != 0)
                {
                    int sum = 0;
                    for (int dy = -1; dy <= 1; ++dy)
                    {
                        for (int dx = -1; dx <= 1; ++dx)
                        {
                            sum += image(x+dx, y+dy);
                        }
                    }
                    blur(x, y) = sum/9;
                }
            }
        }
        image = blur;
    }

    blur.Save("blur.im");

    const int N = 8;
    int* data = new1<int>(blur.GetQuantity());
    memcpy(data, blur.GetData(), blur.GetQuantity()*sizeof(int));

    Climb2D climb(N, data);

    int numVertices, numEdges;
    Vector2f* vertices = 0;
    Climb2D::Edge2* edges = 0;
    climb.ExtractContour(75.5f, 0, numVertices, vertices, numEdges, edges);
    climb.MakeUnique(numVertices, vertices, numEdges, edges);

    std::ofstream outFile("vedata1.txt");
    for (i = 0; i < numVertices; ++i)
    {
        Vector2f& vertex = vertices[i];
        outFile << i << " " << vertex[0] << " , " << vertex[1] << std::endl;
    }
    outFile << std::endl;

    for (i = 0; i < numEdges; ++i)
    {
        Climb2D::Edge2& edge = edges[i];
        outFile << i << " " << edge.first << " " << edge.second << std::endl;
    }

    for (i = 0; i < blur.GetQuantity(); ++i)
    {
        unsigned int gray = (unsigned int)(255.0f*blur[i]/100.0f);
        (*msColor)[i] = (gray | (gray << 8) | (gray << 16));
    }

    // Draw vertices.
    for (i = 0; i < numVertices; ++i)
    {
        Vector2f& vertex = vertices[i];
        x = (int)vertex[0];
        y = (int)vertex[1];
        (*msColor)[x + 257*y] = 0x00FF0000;
    }

    // Draw edges.
    for (i = 0; i < numEdges; i++)
    {
        Climb2D::Edge2& edge = edges[i];
        int x0 = (int)vertices[edge.first][0];
        int y0 = (int)vertices[edge.first][1];
        int x1 = (int)vertices[edge.second][0];
        int y1 = (int)vertices[edge.second][1];
        Line2D(x0, y0, x1, y1, SetPixel);
    }

    msColor->Save("color.im");

    delete1(vertices);
    delete1(edges);
}
//----------------------------------------------------------------------------
void AdaptiveSkeletonClimbing2::SetPixel (int x, int y)
{
    (*msColor)(x, y) = 0x00FF0000;
}
//----------------------------------------------------------------------------
int AdaptiveSkeletonClimbing2::Main (int, char**)
{
    Test0();
    Test1();
    return 0;
}
//----------------------------------------------------------------------------
