// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "SimplePendulum.h"

WM5_CONSOLE_APPLICATION(SimplePendulum);

ImageRGB82D* SimplePendulum::msImage = 0;
unsigned int SimplePendulum::msColor = 0;
float SimplePendulum::msK = 1.0f;

//----------------------------------------------------------------------------
SimplePendulum::SimplePendulum ()
    :
    ConsoleApplication("SamplePhysics/SimplePendulum")
{
}
//----------------------------------------------------------------------------
SimplePendulum::~SimplePendulum ()
{
}
//----------------------------------------------------------------------------
void SimplePendulum::DrawPixel (int x, int y)
{
    if (0 <= x && x < SIZE && 0 <= y && y < SIZE)
    {
        (*msImage)(x, y) = msColor;
    }
}
//----------------------------------------------------------------------------
float* SimplePendulum::ExplicitEuler (float x0, float y0, float h)
{
    float* output = new1<float>(SIZE);
    for (int i = 0; i < SIZE; ++i)
    {
        float x1 = x0 + h*y0;
        float y1 = y0 - h*msK*Mathf::Sin(x0);

        output[i] = x1;
        x0 = x1;
        y0 = y1;
    }
    return output;
}
//----------------------------------------------------------------------------
float* SimplePendulum::ImplicitEuler (float x0, float y0, float h)
{
    const float k0 = msK*h*h;
    float* output = new1<float>(SIZE);
    for (int i = 0; i < SIZE; ++i)
    {
        float k1 = x0 + h*y0;
        float x1 = x0;
        const int maxIteration = 32;
        for (int j = 0; j < maxIteration; ++j)
        {
            float g = x1 + k0*Mathf::Sin(x1) - k1;
            float gDer = 1.0f + k0*Mathf::Cos(x1);
            x1 -= g/gDer;
        }
        float y1 = y0 - h*msK*Mathf::Sin(x1);

        output[i] = x1;
        x0 = x1;
        y0 = y1;
    }
    return output;
}
//----------------------------------------------------------------------------
float* SimplePendulum::RungeKutta (float x0, float y0, float h)
{
    float* output = new1<float>(SIZE);
    for (int i = 0; i < SIZE; ++i)
    {
        float k1X = h*y0;
        float k1Y = -h*msK*Mathf::Sin(x0);
        float x1 = x0 + 0.5f*k1X;
        float y1 = y0 + 0.5f*k1Y;
        float k2X = h*y1;
        float k2Y = -h*msK*Mathf::Sin(x1);
        x1 = x0 + 0.5f*k2X;
        y1 = y0 + 0.5f*k2Y;
        float k3X = h*y1;
        float k3Y = -h*msK*Mathf::Sin(x1);
        x1 = x0 + k3X;
        y1 = y0 + k3Y;
        float k4X = h*y1;
        float k4Y = -h*msK*Mathf::Sin(x1);
        x1 = x0 + (k1X + 2.0f*k2X + 2.0f*k3X + k4X)/6.0f;
        y1 = y0 + (k1Y + 2.0f*k2Y + 2.0f*k3Y + k4Y)/6.0f;

        output[i] = x1;
        x0 = x1;
        y0 = y1;
    }
    return output;
}
//----------------------------------------------------------------------------
float* SimplePendulum::LeapFrog (float x0, float y0, float h)
{
    float* output = new1<float>(SIZE);

    // Generate first iterate with Euler's to start up the process.
    float x1 = x0 + h*y0;
    float y1 = y0 - h*msK*Mathf::Sin(x0);
    output[0] = x1;

    for (int i = 1; i < SIZE; ++i)
    {
        float x2 = x0 + 2.0f*h*y1;
        float y2 = y0 - 2.0f*h*msK*Mathf::Sin(x1);

        output[i] = x2;
        x0 = x1;
        y0 = y1;
        x1 = x2;
        y1 = y2;
    }
    return output;
}
//----------------------------------------------------------------------------
void SimplePendulum::SolveMethod (float* (*method)(float,float,float),
    const char* outImage, const char* outText)
{
    float x0 = 0.1f, y0 = 1.0f;
    float h = 0.1f;

    float* output = method(x0, y0, h);
    std::string path = Environment::GetPathW(outText);
    std::ofstream outFile(path.c_str());
    int i;
    for (i = 0; i < SIZE; ++i)
    {
        outFile << "i = " << i << ", " << output[i] << std::endl;
    }

    // Set image to white.
    for (i = 0; i < msImage->GetQuantity(); ++i)
    {
        (*msImage)[i] = GetColor24(255, 255, 255);
    }

    // Draw the approximate solution.
    float y = 256.0f*(output[0] + 3.0f)/6.0f;
    int iY0 = SIZE - 1 - (int)y;
    for (i = 1; i < SIZE; ++i)
    {
        y = 256.0f*(output[i] + 3.0f)/6.0f;
        int iY1 = SIZE - 1 - (int)y;
        Line2D(i - 1, iY0, i, iY1, DrawPixel);
        iY0 = iY1;
    }

    path = Environment::GetPathW(outImage);
    msImage->Save(path.c_str());

    delete1(output);
}
//----------------------------------------------------------------------------
void SimplePendulum::Stiff1 ()
{
    const int maxIterations = 1024 + 256;
    const float cSqr = 2.0f, c = Mathf::Sqrt(2.0f);

    float h = 0.01f;
    float x0 = 1.0f, x0Save = x0;
    float y0 = -c*x0;

    float* approx = new1<float>(maxIterations);
    int i;
    for (i = 0; i < maxIterations; ++i)
    {
        float k1X = h*y0;
        float k1Y = h*cSqr*x0;
        float x1 = x0 + 0.5f*k1X;
        float y1 = y0 + 0.5f*k1Y;
        float k2X = h*y1;
        float k2Y = h*cSqr*x1;
        x1 = x0 + 0.5f*k2X;
        y1 = y0 + 0.5f*k2Y;
        float k3X = h*y1;
        float k3Y = h*cSqr*x1;
        x1 = x0 + k3X;
        y1 = y0 + k3Y;
        float k4X = h*y1;
        float k4Y = h*cSqr*x1;
        x1 = x0 + (k1X + 2.0f*k2X + 2.0f*k3X + k4X)/6.0f;
        y1 = y0 + (k1Y + 2.0f*k2Y + 2.0f*k3Y + k4Y)/6.0f;

        approx[i] = x1;
        x0 = x1;
        y0 = y1;
    }

    std::string path = Environment::GetPathW("Data/stiff1.txt");
    std::ofstream outFile(path.c_str());
    for (i = 0; i < maxIterations; ++i)
    {
        outFile << "i = " << i << ", " << approx[i] << std::endl;
    }

    // Set image to white.
    for (i = 0; i < msImage->GetQuantity(); ++i)
    {
        (*msImage)[i] = GetColor24(255, 255, 255);
    }

    // Draw the true solution.
    float y = 256.0f*(x0Save + 3.0f)/6.0f;
    int iY0 = SIZE - 1 - (int)y;
    for (i = 1; i < SIZE; ++i)
    {
        int j = (maxIterations - 1)*i/(SIZE - 1);
        y = 256.0f*(x0Save*Mathf::Exp(-c*j*h) + 3.0f)/6.0f;
        int iY1 = SIZE - 1 - (int)y;
        Line2D(i - 1, iY0, i, iY1, DrawPixel);
        iY0 = iY1;
    }

    path = Environment::GetPathW("Data/stiff1_true.im");
    msImage->Save(path.c_str());

    // Set image to white.
    for (i = 0; i < msImage->GetQuantity(); ++i)
    {
        (*msImage)[i] = GetColor24(255, 255, 255);
    }

    // Draw the approximate solution.
    y = 256.0f*(approx[0] + 3.0f)/6.0f;
    iY0 = SIZE - 1 - (int)y;
    for (i = 1; i < SIZE; ++i)
    {
        int j = (maxIterations - 1)*i/(SIZE - 1);
        y = 256.0f*(approx[j] + 3.0f)/6.0f;
        int iY1 = SIZE - 1 - (int)y;
        Line2D(i - 1, iY0, i, iY1, DrawPixel);
        iY0 = iY1;
    }

    path = Environment::GetPathW("Data/stiff1_appr.im");
    msImage->Save(path.c_str());

    delete1(approx);
}
//----------------------------------------------------------------------------
float SimplePendulum::F0 (float t, float x, float y)
{
    return 9.0f*x + 24.0f*y + 5.0f*Mathf::Cos(t) - Mathf::Sin(t)/3.0f;
}
//----------------------------------------------------------------------------
float SimplePendulum::F1 (float t, float x, float y)
{
    return -24.0f*x - 51.0f*y - 9.0f*Mathf::Cos(t) + Mathf::Sin(t)/3.0f;
}
//----------------------------------------------------------------------------
void SimplePendulum::Stiff2True ()
{
    float h = 0.05f;
    float x0 = 4.0f/3.0f;
    float y0 = 2.0f/3.0f;
    float t0 = 0.0f;

    // true solution
    std::string path = Environment::GetPathW("Data/stiff2_true.txt");
    std::ofstream outFile(path.c_str());
    char message[512];
    const int maxIterations = 20;
    for (int i = 0; i <= maxIterations; ++i, t0 += h)
    {
        float e0 = Mathf::Exp(-3.0f*t0);
        float e1 = Mathf::Exp(-39.0f*t0);
        float cDiv3 = Mathf::Cos(t0)/3.0f;
        x0 = 2.0f*e0 - e1 + cDiv3;
        y0 = -e0 + 2.0f*e1 - cDiv3;
        if (i >= 2 && ((i % 2) == 0))
        {
            sprintf(message, "i = %d, %+8.6f, %+8.6f", i, x0, y0);
            outFile << message << std::endl;
        }
    }
}
//----------------------------------------------------------------------------
void SimplePendulum::Stiff2Approximate ()
{
    // Approximation with step size 0.05.
    float h = 0.05f;
    float x0 = 4.0f/3.0f;
    float y0 = 2.0f/3.0f;
    float t0 = 0.0f;

    const int maxIterations = 20;
    float* approx0 = new1<float>(maxIterations + 1);
    float* approx1 = new1<float>(maxIterations + 1);
    approx0[0] = x0;
    approx1[0] = y0;
    int i;
    for (i = 1; i <= maxIterations; ++i)
    {
        float k1X = h*F0(t0, x0, y0);
        float k1Y = h*F1(t0, x0, y0);
        float x1 = x0 + 0.5f*k1X;
        float y1 = y0 + 0.5f*k1Y;
        float k2X = h*F0(t0 + 0.5f*h, x1, y1);
        float k2Y = h*F1(t0 + 0.5f*h, x1, y1);
        x1 = x0 + 0.5f*k2X;
        y1 = y0 + 0.5f*k2Y;
        float k3X = h*F0(t0 + 0.5f*h, x1, y1);
        float k3Y = h*F1(t0 + 0.5f*h, x1, y1);
        x1 = x0 + k3X;
        y1 = y0 + k3Y;
        float k4X = h*F0(t0 + h, x1, y1);
        float k4Y = h*F1(t0 + h, x1, y1);
        x1 = x0 + (k1X + 2.0f*k2X + 2.0f*k3X + k4X)/6.0f;
        y1 = y0 + (k1Y + 2.0f*k2Y + 2.0f*k3Y + k4Y)/6.0f;

        approx0[i] = x1;
        approx1[i] = y1;
        x0 = x1;
        y0 = y1;
        t0 += h;
    }

    std::string path = Environment::GetPathW("Data/stiff2_appr_h0.05.txt");
    std::ofstream outFile(path.c_str());
    char message[512];
    for (i = 0; i <= maxIterations; ++i)
    {
        if ((i % 2) == 0)
        {
            sprintf(message, "i = %d, %+8.6f, %+8.6f", i, approx0[i],
                approx1[i]);
            outFile << message << std::endl;
        }
    }
    outFile.close();

    // Approximation with step size 0.1.
    h = 0.1f;
    x0 = 4.0f/3.0f;
    y0 = 2.0f/3.0f;
    t0 = 0.0f;

    approx0[0] = x0;
    approx1[0] = y0;
    for (i = 1; i <= maxIterations/2; ++i)
    {
        float k1X = h*F0(t0, x0, y0);
        float k1Y = h*F1(t0, x0, y0);
        float x1 = x0 + 0.5f*k1X;
        float y1 = y0 + 0.5f*k1Y;
        float k2X = h*F0(t0 + 0.5f*h, x1, y1);
        float k2Y = h*F1(t0 + 0.5f*h, x1, y1);
        x1 = x0 + 0.5f*k2X;
        y1 = y0 + 0.5f*k2Y;
        float k3X = h*F0(t0 + 0.5f*h, x1, y1);
        float k3Y = h*F1(t0 + 0.5f*h, x1, y1);
        x1 = x0 + k3X;
        y1 = y0 + k3Y;
        float k4X = h*F0(t0 + h, x1, y1);
        float k4Y = h*F1(t0 + h, x1, y1);
        x1 = x0 + (k1X + 2.0f*k2X + 2.0f*k3X + k4X)/6.0f;
        y1 = y0 + (k1Y + 2.0f*k2Y + 2.0f*k3Y + k4Y)/6.0f;

        approx0[i] = x1;
        approx1[i] = y1;
        x0 = x1;
        y0 = y1;
        t0 += h;
    }

    path = Environment::GetPathW("Data/stiff2_appr_h0.10.txt");
    outFile.open(path.c_str());
    for (i = 0; i <= maxIterations/2; ++i)
    {
        sprintf(message, "i = %d, %+8.6f, %+8.6f", i, approx0[i],
            approx1[i]);
        outFile << message << std::endl;
    }
    outFile.close();

    delete1(approx0);
    delete1(approx1);
}
//----------------------------------------------------------------------------
int SimplePendulum::Main (int, char**)
{
    msImage = new0 ImageRGB82D(SIZE, SIZE);

    SolveMethod(ExplicitEuler, "Data/explicit.im", "Data/explicit.txt");
    SolveMethod(ImplicitEuler, "Data/implicit.im", "Data/implicit.txt");
    SolveMethod(RungeKutta, "Data/runge.im", "Data/runge.txt");
    SolveMethod(LeapFrog, "Data/leapfrog.im", "Data/leapfrog.txt");
    Stiff1();
    Stiff2True();
    Stiff2Approximate();

    delete0(msImage);
    return 0;
}
//----------------------------------------------------------------------------
