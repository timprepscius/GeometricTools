// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "ExtractRidges.h"

WM5_CONSOLE_APPLICATION(ExtractRidges);

//----------------------------------------------------------------------------
ExtractRidges::ExtractRidges ()
    :
    ConsoleApplication("SampleImagics/ExtractRidges")
{
}
//----------------------------------------------------------------------------
int ExtractRidges::Main (int, char**)
{
    std::string imageName = Environment::GetPathR("Head.im");
    ImageDouble2D image(imageName.c_str());

    // Normalize the image values to be in [0,1].
    int quantity = image.GetQuantity();
    double minValue = image[0], maxValue = minValue;
    int i;
    for (i = 1; i < quantity; ++i)
    {
        if (image[i] < minValue)
        {
            minValue = image[i];
        }
        else if (image[i] > maxValue)
        {
            maxValue = image[i];
        }
    }
    double invRange = 1.0/(maxValue - minValue);
    for (i = 0; i < quantity; ++i)
    {
        image[i] = (image[i] - minValue)*invRange;
    }

    // Use first-order centered finite differences to estimate the image
    // derivatives.  The gradient is DF = (df/dx, df/dy) and the Hessian
    // is D^2F = {{d^2f/dx^2, d^2f/dxdy}, {d^2f/dydx, d^2f/dy^2}}.
    int xBound = image.GetBound(0);
    int yBound = image.GetBound(1);
    int xBoundM1 = xBound - 1;
    int yBoundM1 = yBound - 1;
    ImageDouble2D dx(xBound, yBound);
    ImageDouble2D dy(xBound, yBound);
    ImageDouble2D dxx(xBound, yBound);
    ImageDouble2D dxy(xBound, yBound);
    ImageDouble2D dyy(xBound, yBound);
    int x, y;
    for (y = 1; y < yBoundM1; ++y)
    {
        for (x = 1; x < xBoundM1; ++x)
        {
            dx(x, y) = 0.5*(image(x+1, y) - image(x-1, y));
            dy(x, y) = 0.5*(image(x, y+1) - image(x, y-1));
            dxx(x, y) = image(x+1, y) - 2.0*image(x, y) + image(x-1, y);
            dxy(x, y) = 0.25*(image(x+1, y+1) + image(x-1, y-1)
                - image(x+1, y-1) - image(x-1, y+1));
            dyy(x, y) = image(x, y+1) - 2.0*image(x, y) + image(x, y+1);
        }
    }
    dx.Save("dx.im");
    dy.Save("dy.im");
    dxx.Save("dxx.im");
    dxy.Save("dxy.im");
    dyy.Save("dyy.im");

    // The eigensolver produces eigenvalues a and b and corresponding
    // eigenvectors U and V:  D^2F*U = a*U, D^2F*V = b*V.  Define
    // P = Dot(U,DF) and Q = Dot(V,DF).  The classification is as follows.
    //   ridge:   P = 0 with a < 0
    //   valley:  Q = 0 with b > 0
    ImageDouble2D aImage(xBound, yBound);
    ImageDouble2D bImage(xBound, yBound);
    ImageDouble2D pImage(xBound, yBound);
    ImageDouble2D qImage(xBound, yBound);
    for (y = 1; y < yBoundM1; ++y)
    {
        for (x = 1; x < xBoundM1; ++x)
        {
            Vector2d gradient(dx(x, y), dy(x, y));
            Matrix2d hessian(dxx(x, y), dxy(x, y), dxy(x, y), dyy(x, y));
            EigenDecompositiond decomposer(hessian);
            decomposer.Solve(true);
            aImage(x,y) = decomposer.GetEigenvalue(0);
            bImage(x,y) = decomposer.GetEigenvalue(1);
            Vector2d u = decomposer.GetEigenvector2(0);
            Vector2d v = decomposer.GetEigenvector2(1);
            pImage(x,y) = u.Dot(gradient);
            qImage(x,y) = v.Dot(gradient);
        }
    }
    aImage.Save("a.im");
    bImage.Save("b.im");
    pImage.Save("p.im");
    qImage.Save("q.im");

    // Use a cheap classification of the pixels by testing for sign changes
    // between neighboring pixels.
    ImageRGB82D result(xBound, yBound);
    for (y = 1; y < yBoundM1; ++y)
    {
        for (x = 1; x < xBoundM1; ++x)
        {
            unsigned char gray = (unsigned char)(255.0f*image(x, y));

            double pValue = pImage(x, y);
            bool isRidge = false;
            if (pValue*pImage(x-1 ,y) < 0.0
            ||  pValue*pImage(x+1, y) < 0.0
            ||  pValue*pImage(x, y-1) < 0.0
            ||  pValue*pImage(x, y+1) < 0.0)
            {
                if (aImage(x, y) < 0.0)
                {
                    isRidge = true;
                }
            }

            double qValue = qImage(x,y);
            bool isValley = false;
            if (qValue*qImage(x-1, y) < 0.0
            ||  qValue*qImage(x+1, y) < 0.0
            ||  qValue*qImage(x, y-1) < 0.0
            ||  qValue*qImage(x, y+1) < 0.0)
            {
                if (bImage(x,y) > 0.0)
                {
                    isValley = true;
                }
            }

            if (isRidge)
            {
                if (isValley)
                {
                    result(x, y) = GetColor24(gray, 0, gray);
                }
                else
                {
                    result(x, y) = GetColor24(gray, 0, 0);
                }
            }
            else if (isValley)
            {
                result(x, y) = GetColor24(0, 0, gray);
            }
            else
            {
                result(x, y) = GetColor24(gray, gray, gray);
            }
        }
    }
    result.Save("result.im");

    return 0;
}
//----------------------------------------------------------------------------
