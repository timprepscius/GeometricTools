// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.6.1 (2012/07/07)

#include "ThinPlateSplines.h"

WM5_CONSOLE_APPLICATION(ThinPlateSplines);

//----------------------------------------------------------------------------
ThinPlateSplines::ThinPlateSplines ()
    :
    ConsoleApplication("SampleMathematics/ThinPlateSplines")
{
}
//----------------------------------------------------------------------------
ThinPlateSplines::~ThinPlateSplines ()
{
}
//----------------------------------------------------------------------------
void TestThinPlateSplines2D ()
{
    FILE* outFile = fopen("output2.txt", "wt");

    // Tabulated data on a 3x3 regular grid, points of form (x,y,f(x,y)).
    const int numPoints = 9;
    double x[numPoints] =
    {
        0.0, 0.5, 1.0,
        0.0, 0.5, 1.0,
        0.0, 0.5, 1.0
    };

    double y[numPoints] =
    {
        0.0, 0.0, 0.0,
        0.5, 0.5, 0.5,
        1.0, 1.0, 1.0
    };

    double f[numPoints] =
    {
        1.0, 2.0, 3.0,
        3.0, 2.0, 1.0,
        1.0, 2.0, 3.0
    };

    // Resample on a 7x7 regular grid.
    const int numResample = 6;
    const double invResample = 1.0/(double)numResample;
    double smooth, interp, functional;
    int i, j;

    // No smoothing, exact interpolation at grid points.
    smooth = 0.0;
    IntpThinPlateSpline2d noSmooth(numPoints, x, y, f, smooth, false, false);
    fprintf(outFile, "no smoothing (smooth parameter is 0.0)\n");
    for (j = 0; j <= numResample; ++j)
    {
        for (i = 0; i <= numResample; ++i)
        {
            interp = noSmooth(invResample*i, invResample*j);
            fprintf(outFile, "%lf ", interp);
        }
        fprintf(outFile, "\n");
    }
    functional = noSmooth.ComputeFunctional();
    fprintf(outFile, "functional = %lg\n", functional);
    fprintf(outFile, "\n");

    // Increasing amounts of smoothing.
    smooth = 0.1;
    for (int k = 1; k <= 6; ++k, smooth *= 10.0)
    {
        IntpThinPlateSpline2d spline(numPoints, x, y, f, smooth, false, false);
        fprintf(outFile, "smoothing (parameter is %lf)\n", smooth);
        for (j = 0; j <= numResample; ++j)
        {
            for (i = 0; i <= numResample; ++i)
            {
                interp = spline(invResample*i, invResample*j);
                fprintf(outFile, "%lf ", interp);
            }
            fprintf(outFile, "\n");
        }
        functional = spline.ComputeFunctional();
        fprintf(outFile, "functional = %lg\n", functional);
        fprintf(outFile, "\n");
    }

    fclose(outFile);
}
//----------------------------------------------------------------------------
void TestThinPlateSplines3D ()
{
    FILE* outFile = fopen("output3.txt", "wt");

    // Tabulated data on a 3x3x3 regular grid, points (x,y,z,f(x,y,z)).
    const int numPoints = 27;
    double x[numPoints], y[numPoints], z[numPoints], f[numPoints];
    double xdomain, ydomain, zdomain;
    int i, j, k, index = 0;
    for (k = 0; k < 3; ++k)
    {
        zdomain = 0.5*k;
        for (j = 0; j < 3; ++j)
        {
            ydomain = 0.5*j;
            for (i = 0; i < 3; ++i, ++index)
            {
                xdomain = 0.5*i;
                x[index] = xdomain;
                y[index] = ydomain;
                z[index] = zdomain;
                f[index] = Mathd::UnitRandom();
            }
        }
    }

    // Resample on a 7x7x7 regular grid.
    const int numResample = 6;
    const double invResample = 1.0/(double)numResample;
    double smooth, interp, functional;

    // No smoothing, exact interpolation at grid points.
    smooth = 0.0;
    IntpThinPlateSpline3d noSmooth(numPoints, x, y, z, f, smooth, false, false);
    fprintf(outFile, "no smoothing (smooth parameter is 0.0)\n");
    for (k = 0; k <= numResample; ++k)
    {
        zdomain = invResample*k;
        for (j = 0; j <= numResample; ++j)
        {
            ydomain = invResample*j;
            for (i = 0; i <= numResample; ++i)
            {
                xdomain = invResample*i;
                interp = noSmooth(xdomain, ydomain, zdomain);
                fprintf(outFile, "%lf ", interp);
            }
            fprintf(outFile, "\n");
        }
        fprintf(outFile, "\n");
    }
    functional = noSmooth.ComputeFunctional();
    fprintf(outFile, "functional = %lg\n", functional);
    fprintf(outFile, "\n");

    // Increasing amounts of smoothing.
    smooth = 0.1;
    for (int ell = 1; ell <= 6; ++ell, smooth *= 10.0)
    {
        IntpThinPlateSpline3d spline(numPoints, x, y, z, f, smooth, false,
            false);
        fprintf(outFile, "smoothing (parameter is %lf)\n", smooth);
        for (k = 0; k <= numResample; ++k)
        {
            zdomain = invResample*k;
            for (j = 0; j <= numResample; ++j)
            {
                ydomain = invResample*j;
                for (i = 0; i <= numResample; ++i)
                {
                    xdomain = invResample*i;
                    interp = spline(xdomain, ydomain, zdomain);
                    fprintf(outFile, "%lf ", interp);
                }
                fprintf(outFile, "\n");
            }
            fprintf(outFile, "\n");
        }
        functional = spline.ComputeFunctional();
        fprintf(outFile, "functional = %lg\n", functional);
        fprintf(outFile, "\n");
    }

    fclose(outFile);
}
//----------------------------------------------------------------------------
int ThinPlateSplines::Main (int, char**)
{
    TestThinPlateSplines2D();
    TestThinPlateSplines3D();
    return 0;
}
//----------------------------------------------------------------------------
