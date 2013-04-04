#include "CpuLocalSolver1.h"
#include "CpuNonlocalSolver1.h"
#include "CpuLocalSolver2.h"
#include "CpuNonlocalSolver2.h"
#include "CpuLocalSolver3.h"
#include "CpuNonlocalSolver3.h"
#include "GpuLocalSolver1.h"
#include "GpuNonlocalSolver1.h"
#include "GpuLocalSolver2.h"
#include "GpuNonlocalSolver2.h"
#include "GpuLocalSolver3.h"
#include "GpuNonlocalSolver3.h"
#include "SaveBMP32.h"

FILE* gOutput = 0;
clock_t gTotal0 = 0, gTotal1 = 0;
clock_t gInner0 = 0, gInner1 = 0;

//---------------------------------------------------------------------------
static void DrawCallback (int x, int y, void* userData)
{
    Image2<PixelBGRA8>& graph = *(Image2<PixelBGRA8>*)userData;
    if (0 <= x && x < graph.GetDimension(0)
    &&  0 <= y && y < graph.GetDimension(1))
    {
        PixelBGRA8& value = graph(x, y);
        value.b = 0;
        value.g = 0;
        value.r = 0;
    }
}
//---------------------------------------------------------------------------
static void DrawLine (int x0, int y0, int x1, int y1,
    void (*callback)(int,int,void*), void* userData)
{
    // Starting point of line.
    int x = x0, y = y0;

    // Direction of line.
    int dx = x1-x0, dy = y1-y0;

    // Increment or decrement depending on direction of line.
    int sx = (dx > 0 ? 1 : (dx < 0 ? -1 : 0));
    int sy = (dy > 0 ? 1 : (dy < 0 ? -1 : 0));

    // Decision parameters for pixel selection.
    if (dx < 0)
    {
        dx = -dx;
    }
    if (dy < 0)
    {
        dy = -dy;
    }
    int ax = 2*dx, ay = 2*dy;
    int decX, decY;

    // Determine largest direction component, single-step related variable.
    int maxValue = dx, var = 0;
    if (dy > maxValue)
    {
        var = 1;
    }

    // Traverse Bresenham line.
    switch (var)
    {
    case 0:  // Single-step in x-direction.
        decY = ay - dx;
        for (/**/; /**/; x += sx, decY += ay)
        {
            callback(x, y, userData);

            // Take Bresenham step.
            if (x == x1)
            {
                break;
            }
            if (decY >= 0)
            {
                decY -= ax;
                y += sy;
            }
        }
        break;
    case 1:  // Single-step in y-direction.
        decX = ax - dy;
        for (/**/; /**/; y += sy, decX += ax)
        {
            callback(x, y, userData);

            // Take Bresenham step.
            if (y == y1)
            {
                break;
            }
            if (decX >= 0)
            {
                decX -= ay;
                x += sx;
            }
        }
        break;
    }
}
//----------------------------------------------------------------------------
void SaveGraph (const std::string& folder, int frame, float ymax,
    int xbound, const float* slice)
{
    const int ybound = 256;
    Image2<PixelBGRA8> graph(xbound, ybound);

    PixelBGRA8* pixels = graph.GetPixels1D();
    for (int i = 0; i < graph.GetNumPixels(); ++i)
    {
        PixelBGRA8& value = *pixels++;
        value.b = 255;
        value.g = 255;
        value.r = 255;
        value.a = 255;
    }

    int x0, y0, x1, y1, cx, cy;
    float t;

    x0 = 0;
    y0 = (int)(ybound*slice[0]/ymax);
    for (x1 = 1; x1 < xbound; ++x1)
    {
        float value = slice[x1];
        if (value < ymax)
        {
            y1 = (int)(ybound*value/ymax);
        }
        else
        {
            y1 = ybound;
        }

        if (y0 < ybound)
        {
            if (y1 < ybound)
            {
                DrawLine(x0, y0, x1, y1, DrawCallback, &graph);
            }
            else
            {
                cy = ybound-1;
                t = (float)(cy - y0)/(float)(y1 - y0);
                cx = (int)(x0 + t*(x1 - x0) + 0.5f);
                DrawLine(x0, y0, cx, cy, DrawCallback, &graph);
            }
        }
        else if (y1 < ybound)
        {
            cy = ybound-1;
            t = (float)(cy - y0)/(float)(y1 - y0);
            cx = (int)(x0 + t*(x1 - x0) + 0.5f);
            DrawLine(cx, cy, x1, y1, DrawCallback, &graph);
        }

        x0 = x1;
        y0 = y1;
    }

    char name[256];
    if (frame < 10)
    {
        sprintf(name, "%s/Frame00%d.bmp", folder.c_str(), frame);
    }
    else if (frame < 100)
    {
        sprintf(name, "%s/Frame0%d.bmp", folder.c_str(), frame);
    }
    else
    {
        sprintf(name, "%s/Frame%d.bmp", folder.c_str(), frame);
    }

    SaveBMP32(name, graph);
}
//---------------------------------------------------------------------------
void SetInitial1 (Image1<float>& initial)
{
    const int n0 = initial.GetDimension(0);
    for (int i0 = 0; i0 < n0; ++i0)
    {
        float x0 = -1.0f + 2.0f*i0/(float)(n0-1);
        initial(i0) = 1.0f - x0*x0;
    }
}
//---------------------------------------------------------------------------
void SetInitial2 (Image2<float>& initial)
{
    const int n0 = initial.GetDimension(0);
    const int n1 = initial.GetDimension(1);
    for (int i1 = 0; i1 < n1; ++i1)
    {
        float x1 = -1.0f + 2.0f*i1/(float)(n1-1);
        float value1 = 1.0f - x1*x1;
        for (int i0 = 0; i0 < n0; ++i0)
        {
            float x0 = -1.0f + 2.0f*i0/(float)(n0-1);
            float value0 = 1.0f - x0*x0;
            initial(i0, i1) = value0*value1;
        }
    }
}
//---------------------------------------------------------------------------
void SetInitial3 (Image3<float>& initial)
{
    const int n0 = initial.GetDimension(0);
    const int n1 = initial.GetDimension(1);
    const int n2 = initial.GetDimension(2);
    for (int i2 = 0; i2 < n2; ++i2)
    {
        float x2 = -1.0f + 2.0f*i2/(float)(n2-1);
        float value2 = 1.0f - x2*x2;
        for (int i1 = 0; i1 < n1; ++i1)
        {
            float x1 = -1.0f + 2.0f*i1/(float)(n1-1);
            float value1 = 1.0f - x1*x1;
            float value12 = value1*value2;
            for (int i0 = 0; i0 < n0; ++i0)
            {
                float x0 = -1.0f + 2.0f*i0/(float)(n0-1);
                float value0 = 1.0f - x0*x0;
                initial(i0, i1, i2) = value0*value12;
            }
        }
    }
}
//---------------------------------------------------------------------------
void SolveCpuLocal1 (const std::string& folder)
{
    std::cout << "Running " << folder.c_str() << std::endl;

    gTotal0 = clock();

    const int n0 = 256;
    const float dt = 0.01f, dx0 = 1.0f;
    const uint64_t numIterations = (1 << 20);
    const int numGaussSeidel = 8;
    bool success;
    Image1<float> initial(n0);
    SetInitial1(initial);
    CpuLocalSolver1 solver(n0, &initial, 0, dt, dx0, folder, success);

    gInner0 = clock();
    solver.Enable();
    for (uint64_t i = 0; i < numIterations && success; ++i)
    {
        success = solver.Execute(i, numGaussSeidel);
    }
    solver.Disable();
    gInner1 = clock();

    gTotal1 = clock();

    fprintf(gOutput, "%s: inner = %d\n", folder.c_str(),
        (int)(gInner1 - gInner0));
    fprintf(gOutput, "%s: total = %d\n", folder.c_str(),
        (int)(gTotal1 - gTotal0));
    fflush(gOutput);
}
//---------------------------------------------------------------------------
void SolveCpuNonlocal1 (const float p, const std::string& folder)
{
    std::cout << "Running " << folder.c_str() << std::endl;

    gTotal0 = clock();

    const int n0 = 256;
    const float dt = 0.01f, dx0 = 1.0f;
    const uint64_t numIterations = (1 << 20);
    const int numGaussSeidel = 8;
    bool success;
    Image1<float> initial(n0);
    SetInitial1(initial);
    CpuNonlocalSolver1 solver(256, &initial, 0, dt, dx0, p, folder, success);

    gInner0 = clock();
    solver.Enable();
    for (uint64_t i = 0; i < numIterations && success; ++i)
    {
        success = solver.Execute(i, numGaussSeidel);
    }
    solver.Disable();
    gInner1 = clock();

    fprintf(gOutput, "%s: inner = %d\n", folder.c_str(),
        (int)(gInner1 - gInner0));
    fprintf(gOutput, "%s: total = %d\n", folder.c_str(),
        (int)(gTotal1 - gTotal0));
    fflush(gOutput);
}
//---------------------------------------------------------------------------
void SolveCpuLocal2 (const std::string& folder)
{
    std::cout << "Running " << folder.c_str() << std::endl;

    gTotal0 = clock();

    const int n0 = 256, n1 = 256;
    const float dt = 0.01f, dx0 = 1.0f, dx1 = 1.0f;
    const uint64_t numIterations = (1 << 20);
    const int numGaussSeidel = 8;
    bool success;
    Image2<float> initial(n0, n1);
    SetInitial2(initial);
    CpuLocalSolver2 solver(n0, n1, &initial, 0, dt, dx0, dx1, folder,
        success);

    gInner0 = clock();
    solver.Enable();
    for (uint64_t i = 0; i < numIterations && success; ++i)
    {
        success = solver.Execute(i, numGaussSeidel);
    }
    solver.Disable();
    gInner1 = clock();

    gTotal1 = clock();

    fprintf(gOutput, "%s: inner = %d\n", folder.c_str(),
        (int)(gInner1 - gInner0));
    fprintf(gOutput, "%s: total = %d\n", folder.c_str(),
        (int)(gTotal1 - gTotal0));
    fflush(gOutput);
}
//---------------------------------------------------------------------------
void SolveCpuNonlocal2 (const float p, const std::string& folder)
{
    std::cout << "Running " << folder.c_str() << std::endl;

    gTotal0 = clock();

    const int n0 = 256, n1 = 256;
    const float dt = 0.01f, dx0 = 1.0f, dx1 = 1.0f;
    const uint64_t numIterations = (1 << 20);
    const int numGaussSeidel = 8;
    bool success;
    Image2<float> initial(n0, n1);
    SetInitial2(initial);
    CpuNonlocalSolver2 solver(256, 256, &initial, 0, dt, dx0, dx1, p,
        folder, success);

    gInner0 = clock();
    solver.Enable();
    for (uint64_t i = 0; i < numIterations && success; ++i)
    {
        success = solver.Execute(i, numGaussSeidel);
    }
    solver.Disable();
    gInner1 = clock();

    gTotal1 = clock();

    fprintf(gOutput, "%s: inner = %d\n", folder.c_str(),
        (int)(gInner1 - gInner0));
    fprintf(gOutput, "%s: total = %d\n", folder.c_str(),
        (int)(gTotal1 - gTotal0));
    fflush(gOutput);
}
//---------------------------------------------------------------------------
void SolveCpuLocal3 (const std::string& folder)
{
    std::cout << "Running " << folder.c_str() << std::endl;

    gTotal0 = clock();

    const int n0 = 256, n1 = 256, n2 = 256;
    const float dt = 0.01f, dx0 = 1.0f, dx1 = 1.0f, dx2 = 1.0f;
    const uint64_t numIterations = (1 << 20);
    const int numGaussSeidel = 8;
    bool success;
    Image3<float> initial(n0, n1, n2);
    SetInitial3(initial);
    CpuLocalSolver3 solver(256, 256, 256, &initial, 0, dt, dx0, dx1, dx2,
        folder, success);

    gInner0 = clock();
    solver.Enable();
    for (uint64_t i = 0; i < numIterations && success; ++i)
    {
        success = solver.Execute(i, numGaussSeidel);
    }
    solver.Disable();
    gInner1 = clock();

    gTotal1 = clock();

    fprintf(gOutput, "%s: inner = %d\n", folder.c_str(),
        (int)(gInner1 - gInner0));
    fprintf(gOutput, "%s: total = %d\n", folder.c_str(),
        (int)(gTotal1 - gTotal0));
    fflush(gOutput);
}
//---------------------------------------------------------------------------
void SolveCpuNonlocal3 (const float p, const std::string& folder)
{
    std::cout << "Running " << folder.c_str() << std::endl;

    gTotal0 = clock();

    const int n0 = 256, n1 = 256, n2 = 256;
    const float dt = 0.01f, dx0 = 1.0f, dx1 = 1.0f, dx2 = 1.0f;
    const uint64_t numIterations = (1 << 20);
    const int numGaussSeidel = 8;
    bool success;
    Image3<float> initial(n0, n1, n2);
    SetInitial3(initial);

    CpuNonlocalSolver3 solver(256, 256, 256, &initial, 0, dt, dx0, dx1,
        dx2, p, folder, success);

    gInner0 = clock();
    solver.Enable();
    for (uint64_t i = 0; i < numIterations && success; ++i)
    {
        success = solver.Execute(i, numGaussSeidel);
    }
    solver.Disable();
    gInner1 = clock();

    gTotal1 = clock();

    fprintf(gOutput, "%s: inner = %d\n", folder.c_str(),
        (int)(gInner1 - gInner0));
    fprintf(gOutput, "%s: total = %d\n", folder.c_str(),
        (int)(gTotal1 - gTotal0));
    fflush(gOutput);
}
//---------------------------------------------------------------------------
void SolveGpuLocal1 (const std::string& folder)
{
    std::cout << "Running " << folder.c_str() << std::endl;

    gTotal0 = clock();

    const int n0 = 256;
    const float dt = 0.01f, dx0 = 1.0f;
    const uint64_t numIterations = (1 << 20);
    const int numGaussSeidel = 8;
    bool success;
    Image1<float> initial(n0);
    SetInitial1(initial);
    GpuLocalSolver1 solver(n0, &initial, 0, dt, dx0, folder, success);

    gInner0 = clock();
    solver.Enable();
    for (uint64_t i = 0; i < numIterations && success; ++i)
    {
        success = solver.Execute(i, numGaussSeidel);
    }
    solver.Disable();
    gInner1 = clock();

    gTotal1 = clock();

    fprintf(gOutput, "%s: inner = %d\n", folder.c_str(),
        (int)(gInner1 - gInner0));
    fprintf(gOutput, "%s: total = %d\n", folder.c_str(),
        (int)(gTotal1 - gTotal0));
    fflush(gOutput);
}
//---------------------------------------------------------------------------
void SolveGpuNonlocal1 (const float p, const std::string& folder)
{
    std::cout << "Running " << folder.c_str() << std::endl;

    gTotal0 = clock();

    const int n0 = 256;
    const float dt = 0.01f, dx0 = 1.0f;
    const uint64_t numIterations = (1 << 20);
    const int numGaussSeidel = 8;
    bool success;
    Image1<float> initial(n0);
    SetInitial1(initial);
    GpuNonlocalSolver1 solver(256, &initial, 0, dt, dx0, p, folder, success);

    gInner0 = clock();
    solver.Enable();
    for (uint64_t i = 0; i < numIterations && success; ++i)
    {
        success = solver.Execute(i, numGaussSeidel);
    }
    solver.Disable();
    gInner1 = clock();

    gTotal1 = clock();

    fprintf(gOutput, "%s: inner = %d\n", folder.c_str(),
        (int)(gInner1 - gInner0));
    fprintf(gOutput, "%s: total = %d\n", folder.c_str(),
        (int)(gTotal1 - gTotal0));
    fflush(gOutput);
}
//---------------------------------------------------------------------------
void SolveGpuLocal2 (const std::string& folder)
{
    std::cout << "Running " << folder.c_str() << std::endl;

    gTotal0 = clock();

    const int n0 = 256, n1 = 256;
    const float dt = 0.01f, dx0 = 1.0f, dx1 = 1.0f;
    const uint64_t numIterations = (1 << 20);
    const int numGaussSeidel = 8;
    bool success;
    Image2<float> initial(n0, n1);
    SetInitial2(initial);

    GpuLocalSolver2 solver(256, 256, &initial, 0, dt, dx0, dx1, folder,
        success);

    gInner0 = clock();
    solver.Enable();
    for (uint64_t i = 0; i < numIterations && success; ++i)
    {
        success = solver.Execute(i, numGaussSeidel);
    }
    solver.Disable();
    gInner1 = clock();

    gTotal1 = clock();

    fprintf(gOutput, "%s: inner = %d\n", folder.c_str(),
        (int)(gInner1 - gInner0));
    fprintf(gOutput, "%s: total = %d\n", folder.c_str(),
        (int)(gTotal1 - gTotal0));
    fflush(gOutput);
}
//---------------------------------------------------------------------------
void SolveGpuNonlocal2 (const float p, const std::string& folder)
{
    std::cout << "Running " << folder.c_str() << std::endl;

    gTotal0 = clock();

    const int n0 = 256, n1 = 256;
    const float dt = 0.01f, dx0 = 1.0f, dx1 = 1.0f;
    const uint64_t numIterations = (1 << 20);
    const int numGaussSeidel = 8;
    bool success;
    Image2<float> initial(n0, n1);
    SetInitial2(initial);

    GpuNonlocalSolver2 solver(256, 256, &initial, 0, dt, dx0, dx1, p,
        folder, success);

    gInner0 = clock();
    solver.Enable();
    for (uint64_t i = 0; i < numIterations && success; ++i)
    {
        success = solver.Execute(i, numGaussSeidel);
    }
    solver.Disable();
    gInner1 = clock();

    gTotal1 = clock();

    fprintf(gOutput, "%s: inner = %d\n", folder.c_str(),
        (int)(gInner1 - gInner0));
    fprintf(gOutput, "%s: total = %d\n", folder.c_str(),
        (int)(gTotal1 - gTotal0));
    fflush(gOutput);
}
//---------------------------------------------------------------------------
void SolveGpuLocal3 (const std::string& folder)
{
    std::cout << "Running " << folder.c_str() << std::endl;

    gTotal0 = clock();

    const int n0 = 256, n1 = 256, n2 = 256;
    const float dt = 0.01f, dx0 = 1.0f, dx1 = 1.0f, dx2 = 1.0f;
    const uint64_t numIterations = (1 << 20);
    const int numGaussSeidel = 8;
    bool success;
    Image3<float> initial(n0, n1, n2);
    SetInitial3(initial);
    GpuLocalSolver3 solver(256, 256, 256, &initial, 0, dt, dx0, dx1, dx2,
        folder, success);

    gInner0 = clock();
    solver.Enable();
    for (uint64_t i = 0; i < numIterations && success; ++i)
    {
        success = solver.Execute(i, numGaussSeidel);
    }
    solver.Disable();
    gInner1 = clock();

    gTotal1 = clock();

    fprintf(gOutput, "%s: inner = %d\n", folder.c_str(),
        (int)(gInner1 - gInner0));
    fprintf(gOutput, "%s: total = %d\n", folder.c_str(),
        (int)(gTotal1 - gTotal0));
    fflush(gOutput);
}
//---------------------------------------------------------------------------
void SolveGpuNonlocal3 (const float p, const std::string& folder)
{
    std::cout << "Running " << folder.c_str() << std::endl;

    gTotal0 = clock();

    const int n0 = 256, n1 = 256, n2 = 256;
    const float dt = 0.01f, dx0 = 1.0f, dx1 = 1.0f, dx2 = 1.0f;
    const uint64_t numIterations = (1 << 20);
    const int numGaussSeidel = 8;
    bool success;
    Image3<float> initial(n0, n1, n2);
    SetInitial3(initial);
    GpuNonlocalSolver3 solver(256, 256, 256, &initial, 0, dt, dx0, dx1,
        dx2, p, folder, success);

    gInner0 = clock();
    solver.Enable();
    for (uint64_t i = 0; i < numIterations && success; ++i)
    {
        success = solver.Execute(i, numGaussSeidel);
    }
    solver.Disable();
    gInner1 = clock();

    gTotal1 = clock();

    fprintf(gOutput, "%s: inner = %d\n", folder.c_str(),
        (int)(gInner1 - gInner0));
    fprintf(gOutput, "%s: total = %d\n", folder.c_str(),
        (int)(gTotal1 - gTotal0));
    fflush(gOutput);
}
//---------------------------------------------------------------------------
void RunConsole ()
{
    gOutput = fopen("timing.txt", "wt");

    SolveGpuLocal1("MoviesConsole/GpuLocal1");
    //SolveGpuNonlocal1(0.50f, "MoviesConsole/GpuNonlocal1_0p50");
    //SolveGpuNonlocal1(0.99f, "MoviesConsole/GpuNonlocal1_0p99");
    //SolveGpuLocal2("MoviesConsole/GpuLocal2");
    //SolveGpuNonlocal2(0.50f, "MoviesConsole/GpuNonlocal2_0p50");
    //SolveGpuNonlocal2(0.99f, "MoviesConsole/GpuNonlocal2_0p99");
    //SolveGpuLocal3("MoviesConsole/GpuLocal3");
    //SolveGpuNonlocal3(0.50f, "MoviesConsole/GpuNonlocal3_0p50");
    //SolveGpuNonlocal3(0.99f, "MoviesConsole/GpuNonlocal3_0p99");

    //SolveCpuLocal1("MoviesConsole/CpuLocal1");
    //SolveCpuNonlocal1(0.50f, "MoviesConsole/CpuNonlocal1_0p50");
    //SolveCpuNonlocal1(0.99f, "MoviesConsole/CpuNonlocal1_0p99");
    //SolveCpuLocal2("MoviesConsole/CpuLocal2");
    //SolveCpuNonlocal2(0.50f, "MoviesConsole/CpuNonlocal2_0p50");
    //SolveCpuNonlocal2(0.99f, "MoviesConsole/CpuNonlocal2_0p99");
    //SolveCpuLocal3("MoviesConsole/CpuLocal3");
    //SolveCpuNonlocal3(0.50f, "MoviesConsole/CpuNonlocal3_0p50");
    //SolveCpuNonlocal3(0.99f, "MoviesConsole/CpuNonlocal3_0p99");

    fclose(gOutput);
}
//---------------------------------------------------------------------------
