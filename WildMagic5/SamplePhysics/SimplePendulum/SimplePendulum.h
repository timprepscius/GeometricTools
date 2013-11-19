// Geometric Tools, LLC
// Copyright (c) 1998-2013
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef SIMPLEPENDULUM_H
#define SIMPLEPENDULUM_H

#include "Wm5ConsoleApplication.h"
#include "Wm5Imagics.h"
using namespace Wm5;

class SimplePendulum : public ConsoleApplication
{
    WM5_DECLARE_INITIALIZE;
    WM5_DECLARE_TERMINATE;

public:
    SimplePendulum ();
    virtual ~SimplePendulum ();

    virtual int Main (int numArguments, char** arguments);

protected:
    enum { SIZE = 512 };

    static float* ExplicitEuler (float x0, float y0, float h);
    static float* ImplicitEuler (float x0, float y0, float h);
    static float* RungeKutta (float x0, float y0, float h);
    static float* LeapFrog (float x0, float y0, float h);
    void SolveMethod (float* (*method)(float,float,float),
        const char* outImage, const char* outText);

    void Stiff1 ();
    float F0 (float t, float x, float y);
    float F1 (float t, float x, float y);
    void Stiff2True ();
    void Stiff2Approximate ();

    static void DrawPixel (int x, int y);
    static ImageRGB82D* msImage;
    static unsigned int msColor;
    static float msK;
};

WM5_REGISTER_INITIALIZE(SimplePendulum);
WM5_REGISTER_TERMINATE(SimplePendulum);

#endif
