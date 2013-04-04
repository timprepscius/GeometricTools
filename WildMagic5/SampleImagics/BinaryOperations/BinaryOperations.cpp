// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.2 (2010/04/30)

#include "BinaryOperations.h"

WM5_CONSOLE_APPLICATION(BinaryOperations);

//----------------------------------------------------------------------------
BinaryOperations::BinaryOperations ()
    :
    ConsoleApplication("SampleImagics/BinaryOperations")
{
}
//----------------------------------------------------------------------------
void BinaryOperations::Get2DBoundaries ()
{
    std::string imageName = Environment::GetPathR("Binary.im");
    ImageInt2D image(imageName.c_str());

    std::vector<std::vector<int> > boundaries;
    Binary2D::GetBoundaries(image, boundaries);
    memset(image.GetData(), 0, image.GetQuantity()*sizeof(int));
    for (int i = 0; i < (int)boundaries.size(); ++i)
    {
        for (int j = 0; j < (int)boundaries[i].size(); ++j)
        {
            image[boundaries[i][j]] = i + 1;
        }
    }
    image.Save("2DBoundaries.im");
}
//----------------------------------------------------------------------------
void BinaryOperations::Get2DComponents8 ()
{
    std::string imageName = Environment::GetPathR("Binary.im");
    ImageInt2D image(imageName.c_str());

    std::vector<std::vector<int> > components;
    Binary2D::GetComponents8(image, false, components);
    memset(image.GetData(), 0, image.GetQuantity()*sizeof(int));
    for (int i = 0; i < (int)components.size(); ++i)
    {
        for (int j = 0; j < (int)components[i].size(); ++j)
        {
            image[components[i][j]] = i;
        }
    }
    image.Save("2DComponents8.im");
}
//----------------------------------------------------------------------------
void BinaryOperations::Get2DComponents4 ()
{
    std::string imageName = Environment::GetPathR("Binary.im");
    ImageInt2D image(imageName.c_str());

    std::vector<std::vector<int> > components;
    Binary2D::GetComponents4(image, false, components);
    memset(image.GetData(), 0, image.GetQuantity()*sizeof(int));
    for (int i = 0; i < (int)components.size(); ++i)
    {
        for (int j = 0; j < (int)components[i].size(); ++j)
        {
            image[components[i][j]] = i;
        }
    }
    image.Save("2DComponents4.im");
}
//----------------------------------------------------------------------------
void BinaryOperations::Get2DL1Distance ()
{
    std::string imageName = Environment::GetPathR("Binary.im");
    ImageInt2D image(imageName.c_str());

    int maxDistance;
    Binary2D::GetL1Distance(image, maxDistance);
    image.Save("2DL1distance.im");
}
//----------------------------------------------------------------------------
void BinaryOperations::Get2DL2Distance ()
{
    std::string imageName = Environment::GetPathR("Binary.im");
    ImageInt2D image(imageName.c_str());

    double maxDistance;
    ImageDouble2D transform(image.GetBound(0), image.GetBound(1));
    Binary2D::GetL2Distance(image, maxDistance, transform);
    transform.Save("2DL2distance.im");
}
//----------------------------------------------------------------------------
void BinaryOperations::Get2DSkeleton ()
{
    std::string imageName = Environment::GetPathR("Binary.im");
    ImageInt2D image(imageName.c_str());

    Binary2D::GetSkeleton(image);
    image.Save("2DSkeleton.im");
}
//----------------------------------------------------------------------------
void BinaryOperations::Get3DComponents26 ()
{
    std::string imageName = Environment::GetPathR("Molecule.im");
    ImageInt3D image(imageName.c_str());

    // Zero-out the boundary and make the image binary.
    const int bound0 = image.GetBound(0);
    const int bound1 = image.GetBound(1);
    const int bound2 = image.GetBound(2);
    int x, y, z;

    z = bound2 - 1;
    for (y = 0; y < bound1; ++y)
    {
        for (x = 0; x < bound0; ++x)
        {
            image(x, y, 0) = 0;
            image(x, y, z) = 0;
        }
    }

    y = bound1 - 1;
    for (z = 0; z < bound2; ++z)
    {
        for (x = 0; x < bound0; ++x)
        {
            image(x, 0, z) = 0;
            image(x, y, z) = 0;
        }
    }

    x = bound0 - 1;
    for (z = 0; z < bound2; ++z)
    {
        for (y = 0; y < bound1; ++y)
        {
            image(0, y, z) = 0;
            image(x, y, z) = 0;
        }
    }

    for (int i = 0; i < image.GetQuantity(); ++i)
    {
        if (image[i])
        {
            image[i] = 1;
        }
    }

    std::vector<std::vector<int> > components;
    Binary3D::GetComponents26(image, false, components);
    memset(image.GetData(), 0, image.GetQuantity()*sizeof(int));
    for (int i = 0; i < (int)components.size(); ++i)
    {
        for (int j = 0; j < (int)components[i].size(); ++j)
        {
            image[components[i][j]] = i;
        }
    }
    image.Save("3DComponents26.im");
}
//----------------------------------------------------------------------------
void BinaryOperations::Get3DComponents18 ()
{
    std::string imageName = Environment::GetPathR("Molecule.im");
    ImageInt3D image(imageName.c_str());

    // Zero-out the boundary and make the image binary.
    const int bound0 = image.GetBound(0);
    const int bound1 = image.GetBound(1);
    const int bound2 = image.GetBound(2);
    int x, y, z;

    z = bound2 - 1;
    for (y = 0; y < bound1; ++y)
    {
        for (x = 0; x < bound0; ++x)
        {
            image(x, y, 0) = 0;
            image(x, y, z) = 0;
        }
    }

    y = bound1 - 1;
    for (z = 0; z < bound2; ++z)
    {
        for (x = 0; x < bound0; ++x)
        {
            image(x, 0, z) = 0;
            image(x, y, z) = 0;
        }
    }

    x = bound0 - 1;
    for (z = 0; z < bound2; ++z)
    {
        for (y = 0; y < bound1; ++y)
        {
            image(0, y, z) = 0;
            image(x, y, z) = 0;
        }
    }

    for (int i = 0; i < image.GetQuantity(); ++i)
    {
        if (image[i])
        {
            image[i] = 1;
        }
    }

    std::vector<std::vector<int> > components;
    Binary3D::GetComponents18(image, false, components);
    memset(image.GetData(), 0, image.GetQuantity()*sizeof(int));
    for (int i = 0; i < (int)components.size(); ++i)
    {
        for (int j = 0; j < (int)components[i].size(); ++j)
        {
            image[components[i][j]] = i;
        }
    }
    image.Save("3DComponents18.im");
}
//----------------------------------------------------------------------------
void BinaryOperations::Get3DComponents6 ()
{
    std::string imageName = Environment::GetPathR("Molecule.im");
    ImageInt3D image(imageName.c_str());

    // Zero-out the boundary and make the image binary.
    const int bound0 = image.GetBound(0);
    const int bound1 = image.GetBound(1);
    const int bound2 = image.GetBound(2);
    int x, y, z;

    z = bound2 - 1;
    for (y = 0; y < bound1; ++y)
    {
        for (x = 0; x < bound0; ++x)
        {
            image(x, y, 0) = 0;
            image(x, y, z) = 0;
        }
    }

    y = bound1 - 1;
    for (z = 0; z < bound2; ++z)
    {
        for (x = 0; x < bound0; ++x)
        {
            image(x, 0, z) = 0;
            image(x, y, z) = 0;
        }
    }

    x = bound0 - 1;
    for (z = 0; z < bound2; ++z)
    {
        for (y = 0; y < bound1; ++y)
        {
            image(0, y, z) = 0;
            image(x, y, z) = 0;
        }
    }

    for (int i = 0; i < image.GetQuantity(); ++i)
    {
        if (image[i])
        {
            image[i] = 1;
        }
    }

    std::vector<std::vector<int> > components;
    Binary3D::GetComponents6(image, false, components);
    memset(image.GetData(), 0, image.GetQuantity()*sizeof(int));
    for (int i = 0; i < (int)components.size(); ++i)
    {
        for (int j = 0; j < (int)components[i].size(); ++j)
        {
            image[components[i][j]] = i;
        }
    }
    image.Save("3DComponents6.im");
}
//----------------------------------------------------------------------------
int BinaryOperations::Main (int, char**)
{
    Get2DBoundaries();
    Get2DComponents8();
    Get2DComponents4();
    Get2DL1Distance();
    Get2DL2Distance();
    Get2DSkeleton();
    Get3DComponents26();
    Get3DComponents18();
    Get3DComponents6();
    return 0;
}
//----------------------------------------------------------------------------
