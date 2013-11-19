// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.5.1 (2012/07/06)

#include "AdaptiveSkeletonClimbing3.h"
#include "Climb3D.h"

WM5_CONSOLE_APPLICATION(AdaptiveSkeletonClimbing3);

//----------------------------------------------------------------------------
AdaptiveSkeletonClimbing3::AdaptiveSkeletonClimbing3 ()
    :
    ConsoleApplication("SampleImagics/AdaptiveSkeletonClimbing3")
{
}
//----------------------------------------------------------------------------
AdaptiveSkeletonClimbing3::~AdaptiveSkeletonClimbing3 ()
{
}
//----------------------------------------------------------------------------
void AdaptiveSkeletonClimbing3::CreateImage (ImageInt3D& image)
{
    // Create an image by sampling a Gaussian distribution.
    int bound = image.GetBound(0);
    float a0 = 256.0f, a1 = 128.0f;
    float x0 = 0.5f*bound, y0 = 0.0f, z0 = 0.0f;
    float x1 = 0.75f*bound, y1 = 0.0f, z1 = 0.0f;
    float xs0 = 2.0f*bound, ys0 = 4.0f*bound, zs0 = 8.0f*bound;
    float xs1 = 8.0f*bound, ys1 = 4.0f*bound, zs1 = 2.0f*bound;

    image = (Eint)0;

    for (int z = 0; z < bound; ++z)
    {
        float vz0 = (z - z0)/zs0, vz1 = (z - z1)/zs1;
        vz0 *= vz0;
        vz1 *= vz1;
        for (int y = 0; y < bound; ++y)
        {
            float vy0 = (y - y0)/ys0, vy1 = (y - y1)/ys1;
            vy0 *= vy0;
            vy1 *= vy1;
            for (int x = 0; x < bound; ++x)
            {
                float vx0 = (x - x0)/xs0, vx1 = (x - x1)/xs1;
                vx0 *= vx0;
                vx1 *= vx1;

                float g0 = a0*Mathf::Exp(-(vx0 + vy0 + vz0));
                float g1 = a1*Mathf::Exp(-(vx1 + vy1 + vz1));
                image(x, y, z) = (int)(g0 + g1);
            }
        }
    }

    image.Save("gauss.im");
}
//----------------------------------------------------------------------------
void AdaptiveSkeletonClimbing3::Test ()
{
    int N = 6, bound = (1 << N) + 1;
    ImageInt3D image(bound, bound, bound);
    CreateImage(image);

    float* data = new1<float>(image.GetQuantity());
    int i;
    for (i = 0; i < image.GetQuantity(); ++i)
    {
        data[i] = (float)image[i];
    }

    Climb3D asc(N, data);
    float level = 349.5f;
    int depth = -1;

    int numVertices, numTriangles;
    Vector3f* vertices = 0;
    TriangleKey* triangles = 0;
    asc.ExtractContour(level, depth, numVertices, vertices, numTriangles,
        triangles);
    asc.MakeUnique(numVertices, vertices, numTriangles, triangles);
    asc.OrientTriangles(vertices, numTriangles, triangles, false);

    Vector3f* normals = asc.ComputeNormals(numVertices, vertices,
        numTriangles, triangles);

    std::ofstream outFile("vtdata.txt");
    outFile << numVertices << std::endl;
    for (i = 0; i < numVertices; ++i)
    {
        Vector3f& vertex = vertices[i];
        outFile << vertex[0] << " " << vertex[1] << " " << vertex[2]
            << std::endl;
    }
    outFile << std::endl;

    for (i = 0; i < numVertices; ++i)
    {
        Vector3f& normal = normals[i];
        outFile << normal[0] << " " << normal[1] << " " << normal[2]
            << std::endl;
    }
    outFile << std::endl;

    outFile << numTriangles << std::endl;
    for (i = 0; i < numTriangles; ++i)
    {
        TriangleKey& triangle = triangles[i];
        outFile << triangle.V[0] << " " << triangle.V[1] << " "
            << triangle.V[2] << std::endl;
    }

    delete1(normals);
    delete1(triangles);
    delete1(vertices);

    asc.PrintBoxes("boxes.txt");
}
//----------------------------------------------------------------------------
int AdaptiveSkeletonClimbing3::Main (int, char**)
{
    Test();
    return 0;
}
//----------------------------------------------------------------------------
